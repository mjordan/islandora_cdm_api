<?php

/**
 * @file
 * Utility functions used by this module.
 */

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
