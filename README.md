# Islandora CONTENTdm API [![Build Status](https://travis-ci.org/mjordan/islandora_cdm_api.png?branch=7.x)](https://travis-ci.org/mjordan/islandora_cdm_api)

## Introduction

This Islandora module provides an API compatible with the [CONTENTdm Web API](http://www.contentdm.org/help6/custom/customize2a.asp). It is intended to provide uninterupted access to content migrated from CONTENTdm to Islandora by allowing client applications consuming the CONTENTdm Web API to continue to function after the migration. In effect this module pretends to be the CONTENTdm web API endpoint. Your apps ask questions of the CONTENTdm API, but the answers come from Islandora.

This module was developed to allow Simon Fraser University Library to continue to use the CONTENTdm Web API on a couple of Drupal websites after they replaced CONTENTdm with Islandora. It will not be of much use to others, except perhaps as an example. Note that in order for it to work, data in your Islandora instance must meet specific requirements, detailed below in the Dependencies section of thie README. You will not be able to use this module without meeting those requirements.

The module is still under development but when complete will implement the following subset of the CONTENTdm API:

* Server level
  * dmGetCollectionList (partial implementation: does not populate the 'path' or 'remote' data)
  * dmGetDublinCoreFieldInfo (partial implementation: JSON output only)
* Collection level
  * dmGetCollectionFieldInfo
  * dmGetCollectionParameters (partial implementation: does not populate the 'path' data and always returns '2' or '-2' in 'rc')
* Item level
  * dmGetCompoundObjectInfo (partial implmentation: XML output only)
  * dmGetImageInfo (partial implementation: does not populate the 'filename' data; XML output only)
  * dmGetItemInfo (partial implementation: JSON output only)
  * dmQuery
  * GetItemDmmodified
  * GetParent
* Utils
  * GetFile
  * GetImage (partial implementation: DMSCALE, DMWIDTH, and DMHEIGHT parameters only)
  * GetStream
  * GetThumbnail

## Dependencies

For the Islandora CONTENTdm API to work, the following conditions must be in place:

* Islandora will need to store the identifiers from the migrated CONTENTdm objects such that this module can query them, for example as CONTENTdm reference URLs in the Islandora objects' MODS metadata (e.g., in an &lt;identifer&gt; element):
```xml
<mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xlink="http://www.w3.org/1999/xlink">
    <!-- Ordinary MODS elements go here -->
    <identifier type="uri" invalid="yes" displayLabel="Migrated From">http://contentdm.example.com/cdm/ref/collection/testcoll1/id/100</identifier>
</mods>
```
* Islandora collection objects corresponding to CONTENTdm collections need a datastream with the DS ID 'CDMFIELDINFO' containing the JSON output of dmGetCollectionFieldInfo for the corresponding collection.
* Islandora collection objects corresponding to CONTENTdm collections need PIDs that use the CONTENTdm collection alias as the namespace, and the string 'collection' as the rest of the PID, e.g., 'art:collection'.
* Islandora objects' MODS datastreams must contain `<extension>` elemements that contain the JSON output of `dmGetItemInfo`, `dmGetCompoundObjectInfo`, and `GetParent`. Specific requirements are documented on the [Move to Islandora Kit wiki](https://github.com/MarcusBarnes/mik/wiki/Metadata-manipulator:-AddContentdmData).
* Client applications that consume the CONTENTdm Web API and that intend to consume the compatible API provided by this module will need to replace base URLs to the API on their CONTENTdm server (e.g., http://contentdm.example.com:81/dmwebservices/index.php?q=) with URLs like http://islandora.sample.com/cdm/api?c=. For requests to the "Utils" (GetFile, GetImage, GetStream, and GetThumbnail), clients will need to replace their "website" base URLs (e.g., http://example.com[:80]) URLs like http://islandora.sample.com/cdm/utils/. The query paths of the requests within the client applications will not need to be changed - only the base URLs.

![How the Islandora CONTENTdm API module works](https://dl.dropboxusercontent.com/u/1015702/linked_to/IslandoraCONTENTdmAPIModuleActivityDiagram.png)

We need to use 'c' as the query parameter in the API URLs because 'q' is used by Drupal for its parameter(s), even with clean URLs enabled.

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

The admin settings form at admin/islandora/tools/cdm_api provides options for setting the source CONTENTdm server hostname and the element in your Solr index that contains each object's CONTENTdm URL.

Also, you will likely want to grant anonymous users the "Access CONTENTdm API" permission.

## Current maintainer

[Mark Jordan](https://github.com/mjordan)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
