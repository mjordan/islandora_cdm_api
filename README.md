# Islandora CONTENTdm API

## Introduction

This Islandora module provides an API compatible with applications that consume the [CONTENTdm Web API](http://www.contentdm.org/help6/custom/customize2a.asp). It implements the following subset of the CONTENTdm API:

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

The module enables applications desinged to consume the CONTENTdm API to issue the same requests to an Islandora instance, and get back the corresonding data, in JSON or XML, for the Islandora object matching the CONTENTdm alias and pointer identified in the request. For this to work, Islandora will need to store the identifiers from the CONTENTdm objects such that this module can query them, for example as CONTENTdm URLs in the Islandora objects' MODS metadata (e.g., in an <identifer> element.

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

## Current maintainer:

* [Mark Jordan](https://github.com/mjordan)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
