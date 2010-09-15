<?php

/**
 * @name _ph_tools_main()
 * @todo turn this into a a theme
 */
function _ph_tools_main() {
	//Header
	$output = '<p>The Custom Enhanacements Module adds many kinds of functionality and customizeable settings to extend Drupal\'s javascript capabilities. To customize, enable or disable any of the enhancements, click on <em>Customize Enhancements</em></p>';
	$output .= '<p>Also, this module contains the following PHP functions, Javascript Functions, and Jquery additions.</p>';
	//PHP functions
	$items = array(
		'ctype_alpha - if the ctype_alpha function does not exist in your php install, this corrects the problem.',
		'ctype_digit - if the ctype_digit function does not exist in your php install, this corrects the problem.',
	);
	$output .= theme('item_list',$items,'PHP Functions','ul');
	//Class Functions
	$items = array(
		'phTools::_strip_views_keys( $str ) - this removes many of the common keys that make for long views lines. Use this to trim your views into manageable pieces.',
		'phTools::_browser_check() - this function returns the current browser, and is available to all modules. returns an array $browser[\'version\'] && $browser[\'type\']',
		'phTools::_list_months() - this function lists all Julian Calendar Months in a nn format, for use in your select boxes!',
		'phTools::_list_years( $startDate, $endDate ) - this function returns the years you specify. ex: return $options = listYears( date( "Y", strtotime("- 3 years") ), date( "Y", strtotime("+3 years") ) );',
		'phTools::_list_countries_iso() - this returns all countries as of 2009. Returns an array, please see the module for the array returned.',
		'phTools::_list_countries_iso_cck() - this returns all countries as of 2009. Returns an array that is easily useable in CCK select fields.',
    'phTools::_list_us_states() - returns current US states as of 2009. This array is compatible with CCK, just return the value from this array.',
		'phTools::_list_canadian_provinces() - returns current Canadian Provinces as of 2009. This array is compatible with CCK, just return the value from this array.',
    'phTools::_get_season( $to ) - returns an array (\'name\',\'date\') and allows you to get the current season ex: phTools::_get_season() or a season in the future phTools::_get_season( 180 )'
	);
	$output .= theme('item_list',$items,'coreEnhancements class static functions','ul');
	//Drupal.js
	$items = array(
		'Browser Detect - accessible on any js enabled page, can access the * BrowserDetect.browser * BrowserDetect.version * BrowserDetect.OS objects, very handy in detecting browsers! From: http://www.quirksmode.org/js/detect.html',
		'Drupal.trace( string ) - sends a string to the browsers console window, like firebug. Since console.log will fail your application in IE, this allows you to safely output to your browsers console.',
		'Drupal.l(  title, ref, attr, destination, id  ) - Emulates the l() function in Drupal along with an id tag and destination. Attributes are rel, classes, alt',
		'Drupal.serialize( data, prefix ) - outputs serialized data into a uriendcoded serialized array, useful for posting data back to drupal',
		'Drupal.urlDecode( encodedString ) - emulates PHP\'s urldecode function',
		'Drupal.saveCookie( name, data, discontinues, domain ) - allows you to save a cookie properly in your browser',
		'Drupal.deleteCookie( name ) - deletes a cookie properly from the system',
		'Drupal.scollTo( target ) - scrolls the browser window to a specific point, much like using anchor tags',
		'Drupal.validateForm( formTarget, pageTarget, showSubmit ) - check a specific div that contains a form to check the fields for unfilled required fields. Also, this will find any email addresses and validate those as well.',
		'Drupal.theme( \'progress\', target, j, total, startMessage, endMessage ) - allows you to create multiple progress bars on one page, emulates theme(\'progress\')',
		'Drupal.settings.drupal_get_destination - returns the current destination from the function drupal_get_destination()'
	);
	$output .= theme('item_list',$items,'Drupal.js javascript accessible functions','ul');
	//Output everything to the markup field
	$form['main'] = array(
    '#type'   => 'markup',
    '#value'  => $output,
    '#weight' => 0,
	);
	return $form;
}

//Customizeable Elements
function _ph_tools_customize() {
	//Main settings
	$form['main_settings'] = array(
		'#type' 				=> 'fieldset',
		'#title' 				=> t('Main Settings'),
		'#collapsible' 	=> FALSE,
		'#collapsed' 		=> FALSE,
		'#weight'				=> -100,
    '#group'        => 'main_settings',
	);
  $form['main_settings']['ph_tools_library_directory'] = array(
    '#type'          => 'textarea',
    '#default_value' => variable_get('ph_tools_library_directory', NULL),
    '#weight'				 => -100,
    '#group'         => 'main_settings',
  );
  $form['main_settings']['ph_tools_global_paths'] = array(
    '#type'          => 'textarea',
    '#default_value' => variable_set('ph_tools_global_paths', "admin*\n"),
    '#weight'				 => -100,
    '#group'         => 'main_settings',
  );
  //krumo(phToolsAdmin::_checkLibrary());
  //Render vertical tabs.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render'; //enabled vertical tabs if the module is installed
  return system_settings_form($form);
}