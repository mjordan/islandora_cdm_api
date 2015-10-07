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
 *   The CONTENTdm object pointer.
 */
function islandora_cdm_api_map_request_to_pid($alias = '', $pointer = '') {
  $solr_url = variable_get('islandora_solr_url', 'http://localhost:8080/solr');
  $source_cdm_server_url = variable_get('islandora_cdm_api_source_cdm_url', 'http://contentdm.example.com');
  $target_element = variable_get('islandora_cdm_api_identifier_element', 'dc.identifier');
  $pid_query = $solr_url . '/select?version=1.2&wt=json&json.nl=map&q=' . $target_element . ':' .
    '%22' . $source_cdm_server_url . '/cdm/ref/collection/' . $alias . '/id/' . $pointer . '%22';
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

/**
 * Query the resource index to get a list of collections.
 *
 * @param string $path
 *   The current URL path.
 * @param string $query
 *   The query following 'c', if there is one.
 */
function islandora_cdm_api_log_requests($path, $query = '') {
  if (variable_get('islandora_cdm_api_log_requests', 0)) {
    $request = $path . $query;
    // Remove 'cdm_api'.
    $request = substr($request, 7);
    $variables = array('%request' => $request);
    watchdog('islandora_cdm_api', '%request', $variables);
  }
}

/**
 * Construct a path to an image cached (or to be cached) on the filesystem.
 *
 * Also creates the cache directory if it doesn't exist.
 *
 * @param string $pid
 *   The PID of the Islandora object.
 * @param string $dsid
 *   The datastream ID.
 * @param string $mimetype
 *   The datastream's MimeType.
 *
 * @return string
 *   The stream wrapper URI for the file.
 */
function islandora_cdm_api_get_cached_image_uri($pid, $dsid, $mimetype) {
  $cache_dir = 'temporary://' . ISLANDORA_CDM_API_CACHE_DIR;
  file_prepare_directory($cache_dir, FILE_CREATE_DIRECTORY);
  module_load_include('inc', 'islandora', 'includes/mimetype.utils');
  $image_filename = preg_replace('/\W/', '_', $pid);
  $image_extension = islandora_get_extension_for_mimetype($mimetype);
  $image_uri = $cache_dir . DIRECTORY_SEPARATOR . $image_filename .
    '_' . $dsid . '.' . $image_extension;
  return $image_uri;
}

/**
 * Confirms that the incoming pixel width, height, and scale
 * values don't conflict.
 *
 * Also creates the cache directory if it doesn't exist.
 *
 * @param object $image
 *   The image object.
 * @param array $params
 *   The request URL parameters.
 *
 * @return 
 *
 */
function islandora_cdm_api_reconcile_scale($image, $params) {

}
