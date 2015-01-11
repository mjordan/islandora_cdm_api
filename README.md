# Islandora CONTENTdm API

## Introduction

This Islandora module provides an API compatible the [CONTENTdm Web API](http://www.contentdm.org/help6/custom/customize2a.asp). It is intended to provide continuing access to content migrated from CONTENTdm to Islandora by allowing client applications consuming the CONTENTdm Web API to continue to function after the migration. In effect this module "pretends" to be the CONTENTdm web API endpoint. Your apps ask questions of the CONTENTdm API, but the answers come from Islandora.

The module implements the following subset of the CONTENTdm API:

* dmGetCollectionList
* dmGetDublinCoreFieldInfo
* dmGetCollectionFieldInfo
* dmGetCollectionParameters
* dmGetCompoundObjectInfo
* dmGetImageInfo
* dmGetItemInfo
* dmQuery
* GetItemDmmodified
* GetParent
* GetFile
* GetImage
* GetStream
* GetThumbnail

For the Islandora CONTENTdm API to work, the following conditions must be in place:

* Islandora will need to store the identifiers from the CONTENTdm objects such that this module can query them, for example as CONTENTdm reference URLs in the Islandora objects' MODS metadata (e.g., in a dc.identifer element).
* Client applications that consume the CONTENTdm Web API and that intend to consume the compatible API provided by this module will need to replace base URLs to the API on their CONTENTdm server (e.g., http://contentdm.example.com:81/dmwebservices/index.php?q=) with URLs like http://islandora.sample.com/cdm_api?c=. For requests to the "Utils" (GetFile, GetImage, GetStream, and GetThumbnail), clients will need to replace their base URLs (e.g., http://example.com[:80]) URLs like http://islandora.sample.com/cdm_api/. The query paths of the requests within the client applications will not need to be changed - only the base URLs.

![How the Islandora CONTENTdm API module works](https://dl.dropboxusercontent.com/u/1015702/linked_to/IslandoraCONTENTdmAPIModuleActivityDiagram.png)

We need to use 'c' as the query parameter in the API URLs because 'q' is used by Drupal, even with clean URLs enabled.

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

The admin settings form at admin/islandora/tools/cdm_api provides options for setting the source CONTENTdm server hostname and the element in your Solr index that contains each object's CONTENTdm URL.

## Current maintainer

[Mark Jordan](https://github.com/mjordan)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
