<?php
/**
 * AJAX Node functions
 * 1. Prepare fields from POST
 * 2. Prepace CCK Objects
 * 3. Get node Object && Process Node Action
 * 4. Return HTML for display
 */

function ce_node_form_cancel_submit($form, &$form_state) {
	drupal_goto();
}

function ce_node_form_add_another_submit($form, &$form_state) {
	drupal_goto('node/add/'.$form['type']['#value']);
}

//process the fields from the $_POST array
function ce_node_process_fields($fields) {
  
}

//prepare node object
function ce_prepare_node_object( $title, $type, $body, $created, $status, $promoted, $sticky, $format, $lang ) {
  
  global $user;
  
  $node           = new stdClass();
  $node->title    = urldecode($title);
  $node->body     = urldecode($body);
  $node->type     = $type;
  $node->created  = $created ? $created : time();
  $node->changed  = $node->created;
  $node->status   = $status ? $status : 1;
  $node->promote  = $promoted ? $promoted : 0;
  $node->sticky   = $sticky ? $sticky : 0;     
  $node->format   = $format ? $format : 1;     
  $node->uid      = $user->uid;   
  $node->language = $lang ? $lang : 'en';
  
  //TODO: CCK Types
  
  //TODO: Location
  
  return $node;
    
}

//Process Node object
function ce_node_process_node($node, $op) {
  
  switch ($op) {
    //save = update
    case 'save':
      
      $node = node_submit($node);
      content_submit($node);
      node_save($node);
      content_insert($node);
      location_save_locations ( $node->locations, array ('nid' => $node->nid, 'vid' => $node->vid ) );
    
    default:
    break;
  
  }
  
  return $node;
  
}

//Return Json to the browser
function ce_node_return_ajax($node) {
  
  $data['node']               = $node;
  $data['node']['validated']  = $node->validated;
  $data['node']['view']       = node_view($node);
  
  drupal_set_header('Content-Type: text/javascript; charset=utf-8');
  coreEnhancements::_set_header_nocache();
  return drupal_to_js( array( 'data' => $data ) );
  
}

/**
 * AJAX Node Functions
 */

//get add form
function ce_get_node_add_form($type) {
	
    if( !function_exists("node_object_prepare")) {
       include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
    }
    return node_add($type);
	
}

//add form callback
function ce_ajax_node_add() {
	
    drupal_set_header('Content-Type: text/javascript; charset=utf-8');
    _set_header_nocache();
    print drupal_to_js( array('form' => _get_node_add_form($_GET['type'])) );
    exit();
	
}

//get edit form
function ce_get_node_edit_form($nid) {
	
    if( !function_exists("node_object_prepare")) {
       include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
    }
	$node = node_load($nid);
    return node_page_edit($node);
	
}

//edit form callback
function ce_ajax_node_edit() {
	
    drupal_set_header('Content-Type: text/javascript; charset=utf-8');
    coreEnhancements::_set_header_nocache();
    print drupal_to_js( array('form' => _get_node_edit_form($_GET['nid'])) );
    exit();
	
}

//submit node callback
function ce_ajax_node_submit() {
	
	//Take variables from $_POST and turn them into the form array
	
	$node = ce_prepare_node_object($title,$type,$body,$created,$status,$promoted,$sticky,$format,$lang);
	
	//process node operation
	$op = $_POST['ajax_op'] ? $_POST['ajax_op'] : 'save';
	$node = ce_node_process_node($node, $op);
	
	//print return data
    print ce_node_return_ajax($node);;
    exit();
	
}