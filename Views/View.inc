<?php

class ViewsBanner {
	public $bannerModel;
	public $filePath;
	public $bannerNid;
	public $bannerUri;

	function __construct(){
		$this->bannerModel = new Model;
		$this->bannerNid = '';
		$this->bannerUri= '';
	}

	function ViewDefaultBanner() {
		//Get the data from the model
		$data = $this->bannerModel->defaultBanner();
		//@todo seems too much work for the view?
		$filePath = file_create_url($data->uri);
		$this->filePath = $filePath;
		return self::bannerOutput();
	}

	function ViewGetBannerWithBannerNodeId($bannerId){
			$this->bannerNid = $bannerId;
			$this->bannerUri = $this->bannerModel->fromBannerNid($this->bannerNid);
			//@todo seems too much work for the view?
			$filePath = file_create_url($this->bannerUri);
			$this->filePath = $filePath;
			return self::bannerOutput();
	}

	function bannerOutput() {
		return "<img class=\"banner\" src=\"{$this->filePath}\">";
	}
}


class ViewsRelatedNodes {
	public $nodeNid;

	function __construct($params) {
		$this->nodeNid = $params['nid'];
	}

	function listOutput() {
		$list = new Model();
		$list = $list->getRelatedNodes($this->nodeNid);
		if(isset($list)) {
			foreach($list as $key => $value) {
				$listOut[$key] = l($value, 'node/'.$key, array('attributes' => array('target' => '_blank')));
			}
			$list = theme('item_list', array(
				'title' => 'Related Nodes',
				'items' => $listOut,
			));
		} else {
			$list = theme('item_list', array(
				'title' => 'Related Nodes',
				'items' => array(),
			));;
		}
		return $list;
	}


}