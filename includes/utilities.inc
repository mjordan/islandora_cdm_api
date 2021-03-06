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
 * Performs a browse or search query against Islandora's Solr index.
 *
 * Returns a set of records.
 *
 * @param string $query
 *   The query string.
 *
 * @return array
 *   List of records from Solr formatted as CONTENTdm's native record set.
 */
function islandora_cdm_api_query_solr($query) {
  $solr_url = variable_get('islandora_solr_url', 'http://localhost:8080/solr');
  $query = $solr_url . '/select?version=1.2&wt=json&json.nl=map&q=' . $query;
  $raw_result = drupal_http_request($query);
  // dd($raw_result);
  if ($raw_result->code != 200) {
    return FALSE;
  }
  $query_result = json_decode($raw_result->data);
  if ($query_result->response->numFound < 1) {
    return FALSE;
  }

  // Convert the records in the format that the client expects.
  $fake_cdm_response = array();
  $fake_cdm_records = array();
  $docs = $query_result->response->docs;
  foreach ($docs as $doc) {
    $doc = (array) $doc;
    // dd($doc, 'Doc');
    // Since fields are multivalued in Solr we just take the first one.
    $fake_cdm_record = array();
    if (isset($doc['dc.title'])) {
      $fake_cdm_record['title'] = $doc['dc.title'][0];
    }
    else {
      $fake_cdm_record['title'] = array();
    }
    if (isset($doc['dc.creator'])) {
      $fake_cdm_record['creato'] = $doc['dc.creator'][0];
    }
    else {
      $fake_cdm_record['creato'] = array();
    }
    if (isset($doc['dc.date'])) {
      $fake_cdm_record['date'] = $doc['dc.date'][0];
    }
    else {
      $fake_cdm_record['date'] = array();
    }
    if (isset($doc['dc.subject'])) {
      $fake_cdm_record['subjec'] = $doc['dc.subject'][0];
    }
    else {
      $fake_cdm_record['subjec'] = array();
    }
    $fake_cdm_records[] = $fake_cdm_record;
  }
  $fake_cdm_response['records'] = $fake_cdm_records;
  return $fake_cdm_response;
}



/**
 * Query the resource index to get a list of collections.
 *
 * @todo: Filter out other unwanted collections.
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
 * Log API requests.
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
 * Determine the pixel width and height to pass to image_imagemagick_resize().
 *
 * Tests against the CONTENTdm API show that CONTENTdm scales the image before
 * it resizes it to the specified pixel width and height. In other words, it
 * performs two separate resizing operations. However, this module can only
 * (for performnance reasons) perform a single resizing operation. SFU's use
 * case is limited to retrieving a version of the image at original size or
 * at a smaller size with the original aspect ratio. To work around this
 * conflict such that we can provide image_imagemagick_resize() with one set
 * of specific pixel dimensions, we need to inspect the value of DMSCALE, and
 * if it is different than 100%, adjust the incoming width and height values
 * accordingly to hand off to image_resize().
 *
 * @param object $image
 *   The image object.
 * @param array $params
 *   The request GetImage URL parameters.
 *
 * @return array
 *   An associative array containing the adjusted width and height values.
 */
function islandora_cdm_api_get_resize_values($image, array $params) {
  $dimensions = array();
  if ($params['DMSCALE'] == '100') {
    $dimensions['width'] = $params['DMWIDTH'];
    $dimensions['height'] = $params['DMHEIGHT'];
  }
  else {
    $dimensions['width'] = round($params['DMWIDTH'] * $params['DMSCALE'] / 100);
    $dimensions['height'] = round($params['DMHEIGHT'] * $params['DMSCALE'] / 100);
  }
  return $dimensions;
}

/**
 * Takes in a string that contains one mapping per line, returns an array.
 *
 * @param string $string
 *   The string to split into an array.
 * @parem string $delimiter
 *   The delimiter.
 *
 * @return array
 *   An associative array created from the string.
 */
function islandora_cdm_api_get_mapping_from_string($string, $delimiter) {
  $map = array();
  $list = preg_split('/$\R?^/m', $string);
  foreach ($list as $mapping) {
    list($left, $right) = explode($delimiter, $mapping);
    $left = trim($left);
    $right = trim($right);
    if (strlen($left)) {
      $map[$left] = $right;
    }
  }
  return $map;
}
