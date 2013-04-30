<?php

class ControllerBanner {
	public $path;
	public $bannerId;
	public $banner;
	public $bannerNodeId;


	function __construct($path) {
		$this->path = $path;
		$this->bannerId = '';
		$this->bannerNodeId = '';
		$this->bannerFileObject = '';
		//Dependencies
		$this->banner = new ViewsBanner();
	}


	function checkForBanner($path) {
		$this->bannerId = db_query("SELECT bannernid as bannerId FROM banner_relations WHERE entity = :path", array(':path' => $path))->fetchField();
		return $this->bannerId;
	}


	function returnBanner() {
		$banner = self::checkForBanner($this->path);
		if(empty($banner)) {
			//Get Default Banner
			$defaultBanner = $this->banner->ViewDefaultBanner();
			return $defaultBanner;
		} else {
			$this->bannerNodeId = $banner;
			$bannerOutput = $this->banner->ViewGetBannerWithBannerNodeId($this->bannerNodeId);
			return $bannerOutput;
		}
	}
}

class bannerFieldController {
	public $type;
	public $form_id;
	public $multi;
	public $nodeId;
	public $defaults;

	function __construct($args) {
			if(is_numeric($args['path'])) {
				$this->nodeId = $args['path'];
			} else {
				$this->nodeId = 'new';
			}
	}

	function getExistingRelationSingle(){
		if($this->type != 'banner') {
			$query = db_query("SELECT bannernid FROM banner_relations
													INNER JOIN node n ON n.nid = bannernid
													WHERE entity = :id ORDER BY title",
													array(':id' => $this->nodeId))->fetchField();
			$this->defaults = $query;
			return $this->defaults;
		} else {

		}
	}

	function buildRelationsFormField($key) {
		$form = '';
		$this->type = $key['type'];
		$this->multi = $key['multi'];
		if($this->type != 'banner') {
			$form = self::buildNonBannerForm();
		} else {
			$form = self::buildBannerForm();
		}
		return $form;
	}

	function buildNonBannerForm(){
		$default_values = self::getExistingRelationSingle();
		$bannerList = self::getBannerRadioList();
		$form['bannergroup'] = array(
			'#type' => 'container',
			'#weight' => 2,
			'#prefix' => '<div class="container span10"><div class="row">',
			'#suffix' => '</div></div>',
		);
		$form['bannergroup']['banner'] = array(
			'#type' => 'radios',
			'#title' => t('Select or remove related banner'),
			'#default_value' => !empty($default_values) ? $default_values : 0,
			'#options' => $bannerList,
			'#required' => FALSE,
			'#prefix' => '<div class="span5">',
			'#suffix' => '</div>',
		);
		$form['bannergroup']['banner_chosen'] = array(
			'#markup' => '
					<div class="banner-chosen">
						<h4>Chosen Banner</h4>
					</div>
					<div class="chosen-banner span4"></div>',
		);
		return $form;
	}

	function buildBannerForm(){
		//Get banners alreadye existing relations
		$default_values = self::findBannerDefaults();
		//find all other nodes in the target group
		$targets = self::getPossibleNodes();
		$form['bannergroup-banner'] = array(
			'#type' => 'container',
			'#weight' => 2,
			'#prefix' => '<div class="container span10"><div class="row">',
			'#suffix' => '</div></div>',
		);
		$form['bannergroup-banner']['bannernodes'] = array(
			'#type' => 'select',
			'#multiple' => TRUE,
			'#title' => t('Select or remove related banner'),
			'#default_value' => !empty($default_values) ? $default_values : 0,
			'#options' => $targets,
			'#required' => FALSE,
			'#prefix' => '<div class="span5">',
			'#suffix' => '</div>',
		);
		return $form;
	}

	function findBannerDefaults() {
		$nid = $this->nodeId;
		$query = db_query("SELECT entity FROM banner_relations WHERE bannernid = :nid AND bannernid != 0", array(':nid' => $nid))->fetchAllKeyed(0, 0);
		return $query;
	}

	function getBannerRadiolist() {
		$options = array();
		//@todo move into model
		$query = db_query("SELECT nid, uri FROM node n
												INNER JOIN field_data_field_banner fb ON fb.entity_id = n.nid
												INNER JOIN file_managed fm ON fm.fid = field_banner_fid
												WHERE n.type LIKE 'banner'
												and n.status = 1 ORDER by title")->fetchAllKeyed(0, 1);
		$options[0] = "<div class=\"banner-default\" data-toggle=\"popover\" title=\"Setting Default Image\" data-original-title=\"Setting Default Image\"
			>Use Default</div>";
		foreach($query as $key => $value) {
			$image = file_create_url($value);
			$options[$key] = "<img width='300' src=\"{$image}\">";
		}
		return $options;
	}

	function getPossibleNodes(){
		$targets = banner_targets();
		foreach($targets as $key) {
			if($key['type'] != 'banner') {
				$nicename = $key['nicename'];
				$type = $key['type'];
				$options[$nicename] = self::getNodesByType($type);
			}
		}
		return $options;
	}

	function getNodesByType($type) {
		//@todo move into model
		$query = db_query("SELECT nid, title FROM node WHERE type LIKE :type ANd status = 1 ORDER by title", array(':type' => $type))->fetchAllKeyed(0, 1);
		return $query;
	}


}

class bannerProcessController {
	public $targets;
	public $action;
	public $node;

	function __construct($params){
		$this->targets = $params['targets'];
		$this->action = $params['action'];
		$this->node = $params['node'];
	}


	function update() {
		$node = $this->node;
		foreach($this->targets as $key) {
			if($key['type'] == $node->type) {
				if($node->type != 'banner') {
					self::updateNode($node);
				} else {
					self::updateBanner($node);
				}
			}
		}
	}

	function insert() {
		$node = $this->node;
		foreach($this->targets as $key) {
			if($key['type'] == $node->type) {
				if($node->type != 'banner') {
					self::insertNode($node);
				} else {
					self::insertBanner($node);
				}
			}
		}
	}

	function delete() {
		$node = $this->node;
		foreach($this->targets as $key) {
			if($key['type'] == $node->type) {
				if($node->type != 'banner') {
					self::deleteNodeBannerRows($node);
				} else {
					self::deleteBannerNodeRows($node);
				}
			}
		}
	}

	function updateNode($node) {
		db_delete('banner_relations')
			->condition('entity', $node->nid)
			->execute();
		$bannerChosen = $node->banner;
		db_insert('banner_relations')
			->fields(array(
			'pid' => NULL,
			'bannernid' => ($bannerChosen) ? $bannerChosen : 0,
			'entity' => $node->nid,
		))
			->execute();
	}

	function updateBanner($node) {
		db_delete('banner_relations')
			->condition('bannernid', $node->nid)
			->execute();

		$nodesChosen = $node->bannernodes;
		//Also need to make sure the nodes it relates to are ready for this
		//one Banner relationship
		self::bannerClean($nodesChosen);
		foreach($nodesChosen as $nid) {
			db_insert('banner_relations')
				->fields(array(
				'pid' => NULL,
				'bannernid' => $node->nid,
				'entity' => ($nid) ? $nid : 0,
			))
				->execute();
		}
	}

	function insertNode($node) {
		$bannerChosen = $node->banner;
		db_insert('banner_relations')
			->fields(array(
			'pid' => NULL,
			'bannernid' => ($bannerChosen) ? $bannerChosen : 0,
			'entity' => $node->nid,
		))
			->execute();
	}

	function insertBanner($node) {
		$nodesChosen = $node->bannernodes;
		self::bannerClean($nodesChosen);
		foreach($nodesChosen as $nid) {
			db_insert('banner_relations')
				->fields(array(
				'pid' => NULL,
				'bannernid' => $node->nid,
				'entity' => ($nid) ? $nid : 0,
			))
				->execute();
		}
	}

	function deleteNodeBannerRows($node) {
		db_delete('banner_relations')
			->condition('entity', $node->nid)
			->execute();
	}

	function deleteBannerNodeRows($node) {
		db_delete('banner_relations')
			->condition('bannernid', $node->nid)
			->execute();
	}

	function bannerClean($nodesChosenArray) {
		foreach($nodesChosenArray as $key => $value){
			db_delete('banner_relations')
			->condition('entity', $value)
			->execute();
		}
	}

}