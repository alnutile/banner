<?php

class Model {
	public $bannerUri;

	function __construct(){
		$this->bannerUri = '';
	}

	function defaultBanner() {
		$query = db_query("SELECT data from field_config WHERE field_name = 'field_banner'")->fetchField();
		if(isset($query)) {
			$query = unserialize($query);
			$fid = $query['settings']['default_image'];
			$file = file_load($fid);
			return $file;
		} else {
			return FALSE;
		}
	}

	function fromBannerNid($bannerId) {
		$query = db_query("SELECT uri FROM file_usage fu INNER JOIN file_managed fm ON fm.fid = fu.fid
			WHERE id = :bannerNodeId", array(':bannerNodeId' => $bannerId))->fetchField();
		$this->bannerUri = $query;
		return $this->bannerUri;
	}

	function getRelatedNodes($nid) {
		$query = db_query("SELECT nid, title
							FROM banner_relations b
							INNER JOIN node n ON n.nid = b.entity
							WHERE bannernid = :nid ORDER by n.title", array(':nid' => $nid))->fetchAllKeyed(0,1);
		return $query;
	}

}