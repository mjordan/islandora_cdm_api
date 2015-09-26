<?php

/**
 * @file
 * Utility functions used by this module.
 */

/**
 * Performs a query to Islandora's Solr index to get a PID.
 *
 * The object is the one corresponding to the alias and pointer.
 *
 * @param string $alias
 *   The CONTENTdm collection alias.
 * @param string $pointer
 *   The CONENTdm object pointer.
 */
function islandora_cdm_api_map_request_to_pid($alias = '', $pointer = '') {
  $solr_url = variable_get('islandora_solr_url', 'http://localhost:8080/solr');
  $source_cdm_server_url = variable_get('islandora_cdm_api_source_cdm_url', 'http://contentdm.example.com');
  $target_element = variable_get('islandora_cdm_api_identifier_element', 'dc.identifier');
  $pid_query = $solr_url . '/select?version=1.2&wt=json&json.nl=map&q=' . $target_element . ':' .
    '%22' . $source_cdm_server_url . '/cdm/ref/collection/' . $alias . '/id/' . $pointer . '%22';
  error_log($pid_query . "\n", 3, '/home/travis/apierror.log');
  $raw_result = drupal_http_request($pid_query);
  if ($raw_result->code != 200) {
    return FALSE;
  }
  $query_result = json_decode($raw_result->data);
  if ($query_result->response->numFound < 1) {
    return FALSE;
  }
  // If more than one object was found, take the PID of the first one.
  return $query_result->response->docs[0]->PID;
}

/**
 * Query the resource index to get a list of collections.
 *
 * @return array
 *   List of PIDs corresponding to the collections or content models.
 */
function islandora_cdm_api_get_collections() {
  $type_for_query = '<info:fedora/islandora:collectionCModel>';
  // Query the rindex to get all collections.
  $tuque = islandora_get_tuque_connection();
  $ri_query = "select \$object \$label from <#ri>
     where \$object <fedora-model:hasModel> $type_for_query
      and \$object <fedora-model:label> \$label";
  $results = $tuque->repository->ri->itqlQuery($ri_query, 'unlimited');
  $ret = array();
  foreach ($results as $member) {
    $pid = preg_replace('/info:fedora\//', '', $member['object']['value']);
    $label = $member['label']['value'];
    // We don't want the root collection.
    if ($pid == 'islandora:root') {
      $label = 'Islandora Root Collection';
    }
    $ret[$pid] = $label;
  }
  return $ret;
}
