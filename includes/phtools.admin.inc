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
	$output = theme('item_list',$items,'PHP Functions','ul');
	$form['help_functions'] = array(
	'#type' 		=> 'fieldset',
	'#title' 		=> t('PHP Functions'),
	'#collapsible' 	=> FALSE,
	'#collapsed' 	=> FALSE,
    '#value'        => $output,
    '#weight'       => -100,
    '#group'        => 'functions',
	);
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
	$output = theme('item_list',$items,'PHTools static functions','ul');
	$form['help_classes'] = array(
	'#type' 		=> 'fieldset',
	'#title' 		=> t('Static Module Helper Functions'),
	'#collapsible' 	=> FALSE,
	'#collapsed' 	=> FALSE,
    '#value'        => $output,
    '#weight'       => -99,
    '#group'        => 'classes',
	);
	//Drupal.PHTools.js
	$items = array(
	'Browser Detect - accessible on any js enabled page, can access the * BrowserDetect.browser * BrowserDetect.version * BrowserDetect.OS objects, very handy in detecting browsers! From: http://www.quirksmode.org/js/detect.html',
	'Drupal.PHTools.trace( string ) - sends a string to the browsers console window, like firebug. Since console.log will fail your application in IE, this allows you to safely output to your browsers console.',
	'Drupal.PHTools.l(  title, ref, attr, destination, id  ) - Emulates the l() function in Drupal along with an id tag and destination. Attributes are rel, classes, alt',
	'Drupal.PHTools.serialize( data, prefix ) - outputs serialized data into a uriendcoded serialized array, useful for posting data back to drupal',
	'Drupal.PHTools.urlDecode( encodedString ) - emulates PHP\'s urldecode function',
	'Drupal.PHTools.saveCookie( name, data, discontinues, domain ) - allows you to save a cookie properly in your browser',
	'Drupal.PHTools.deleteCookie( name ) - deletes a cookie properly from the system',
	'Drupal.PHTools.scollTo( target ) - scrolls the browser window to a specific point, much like using anchor tags',
	'Drupal.PHTools.validateForm( formTarget, pageTarget, showSubmit ) - check a specific div that contains a form to check the fields for unfilled required fields. Also, this will find any email addresses and validate those as well.',
	'Drupal.PHTools.theme( \'progress\', target, j, total, startMessage, endMessage ) - allows you to create multiple progress bars on one page, emulates theme(\'progress\')',
	'Drupal.PHTools.settings.drupal_get_destination - returns the current destination from the function drupal_get_destination()'
	);
	$output = theme('item_list',$items,'Drupal.PHTools.js javascript accessible functions','ul');
	$form['help_javscript'] = array(
	'#type' 		=> 'fieldset',
	'#title' 		=> t('Javascript Helper Functions'),
	'#collapsible' 	=> FALSE,
	'#collapsed' 	=> FALSE,
    '#value'        => $output,
    '#weight'       => -98,
    '#group'        => 'javascript',
	);
  //Render vertical tabs.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render'; //enabled vertical tabs if the module is installed
	return $form;
}

//Customizeable Elements
function _ph_tools_customize() {
  //Set draggable UI.
  jquery_ui_add('sortable');
  //Add admin.css
  drupal_add_css(PH_TOOLS_MODULE_PATH .'/css/admin.elements.css');
  //Add JS for this element.
  drupal_add_js(PH_TOOLS_MODULE_PATH .'/js/behaviors.admin.elements.js');
  //Geocoding API keys.
	$form['location'] = array(
    '#type' 		   => 'fieldset',
    '#title' 		   => t('Location API Keys'),
    '#collapsible' => FALSE,
    '#collapsed' 	 => FALSE,
    '#weight'		   => -100,
    '#group'       => 'location',
	);
  $form['location']['phtools_map_key_google_geocoder'] = array(
    '#type'          => 'textfield',
    '#title' 		     => t('Google Map API Key'),
    '#description' 	 => t('Please enter your google api key here. While this is not required, you cannot use the phTools::_geoCode functionality without it.'),
    '#default_value' => variable_get('phtools_map_key_google_geocoder', NULL),
    '#required'      => FALSE,
    '#weight'		     => -100,
    '#group'         => 'location',
  );
  $form['location']['phtools_map_key_yahoo_geocoder'] = array(
    '#type'          => 'textfield',
    '#title' 		     => t('Yahoo Map API Key'),
    '#description' 	 => t('Please enter the Yahoo API key to allow the Yahoo API to geocode your query. It is recommended that if you enter a google map api key, that you also enter a Yahoo API key, in case google denies you request, or cannot find the location.'),
    '#default_value' => variable_get('phtools_map_key_yahoo_geocoder', NULL),
    '#required'      => FALSE,
    '#weight'		     => -100,
    '#group'         => 'location',
  );
  //Check the library information and set the current libraries.
	$form['jquery_libraries'] = array(
    '#type' 		=> 'fieldset',
    '#title' 		=> t('Jquery Libraries'),
    '#collapsible' 	=> FALSE,
    '#collapsed' 	=> FALSE,
    '#weight'		=> 0,
    '#group'        => 'jquery_libraries',
	);
  $form['jquery_libraries']['ph_tools_library_directory'] = array(
    '#type'          => 'textfield',
    '#title' 		 => t('Directory Path for Jquery Libraries'),
    '#description' 	 => t('This defaults to sites/all/libraries and you may change this, if you do it will rescan the directories for your jquery libraries.'),
    '#default_value' => variable_get('ph_tools_library_directory', NULL),
    '#required'      => TRUE,
    '#weight'		 => -100,
    '#group'         => 'jquery_libraries',
  );
  $form['jquery_libraries']['ph_tools_library_whitelist'] = array(
    '#type'          => 'textarea',
    '#title' 		 => t('Library Whitelist'),
    '#description' 	 => t('You must add the file names you wish to use ino this list. Please add one file name per line.'),
    '#default_value' => variable_get('ph_tools_library_whitelist', PH_TOOLS_MODULE_WHITELIST),
    '#maxlength'     => 1000,
    '#rows'          => 10,
    '#required'      => TRUE,
    '#weight'		 => -100,
    '#group'         => 'jquery_libraries',
  );
  $form['jquery_libraries']['libraries']['#prefix'] = '<h3 class="title">'.t('Script Library (sort by dragging fieldsets)').'</h3><div class="ui-wrapper"><ul id="phtools-sortable-library">';
  $form['jquery_libraries']['libraries']['#suffix'] = '</ul></div>';
  $form['jquery_libraries']['libraries']['library'] = phToolsAdmin::_checkLibrary();
  //Options, for enable and disable
  $options = array('DISABLE','ENABLE');
	//Main settings
	$form['main_settings'] = array(
    '#type' 		=> 'fieldset',
    '#title' 		=> t('Global Settings'),
    '#collapsible' 	=> FALSE,
    '#collapsed' 	=> FALSE,
    '#weight'		=> 100,
    '#group'        => 'main_settings',
	);
  $form['main_settings']['ph_tools_global_paths'] = array(
    '#type'          => 'textarea',
    '#title' 		 => t('Global path settings'),
    '#description' 	 => t('You can set which pages not to have the extended Drupal.PHTools scripts run. You can use <front>, path/* */path *foo* or negate with a ~ like ~<front> ~path/* ~*/path ~*path*.'),
    '#default_value' => variable_get('ph_tools_global_paths', PH_TOOLS_MODULE_GLOBAL_PATHS),
    '#weight'		 => -100,
    '#group'         => 'main_settings',
  );
  //Fade classes - This is primarily used for fading drupal_set_message messages, but can be used creatively for sure.
	$form['main_settings']['phtools_fade_messages_override'] = array(
	  '#title' 			=> 'Enable or disable fading of messages',
	  '#description' 	=> t('Drupal messages stick to the page. This setting allows status message to fade.'),
	  '#type' 			=> 'radios',
	  '#default_value' 	=> variable_get('phtools_fade_messages_override', 0),
	  '#options' 		=> $options,
    '#weight'         => 1,
    '#group'          => 'main_settings'
	);
  //Timeouts.
	$form['main_settings']['ph_tools_fade_out_delay'] = array(
	  '#title' 			=> 'Timeout before fade happens',
	  '#description' 	=> 'Enter the timeout value, in miliseconds before the timeout fires the fade out',
	  '#type' 			=> 'textfield',
	  '#default_value' 	=> variable_get('ph_tools_fade_out_delay',5500),
    '#weight'         => 3,
    '#group'          => 'main_settings'
	);
  //Add our own validate handler.
  $form['#validate'][]   = '_ph_tools_customize_validate';
  //Render vertical tabs.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render'; //enabled vertical tabs if the module is installed
  return system_settings_form($form);
}

//Validate the form we submitted.
function _ph_tools_customize_validate($form_id, &$form_state) {
  //Make sure our white list strips out spaces at the very least. @todo Make this reconize files existing.
  $form_state['values']['ph_tools_library_whitelist'] = str_replace(' ','',$form_state['values']['ph_tools_library_whitelist']);
  //Update our library settings, so we can just get them quikly and go.
  $library = variable_get('ph_tools_library', array());
  foreach ($form_state['values'] as $key => $value) {
    if (strstr($key,'ph_tools_enable_') && file_exists($value)) {
      //Set our library scripts by weight, and sort them before we save.
      $library[(int)$form_state['values'][$key .'_weight']] = array(
        'script' => $value,
        'paths'  => $form_state['values'][$key .'_paths'],
        'scope'  => $form_state['values'][$key .'_scope'],
        'weight' => $form_state['values'][$key .'_weight'],
      );
    }
  }
  //Save Library.
  variable_set('ph_tools_library', $library);
}

//Location Information.
function _ph_tools_location() {
  //Geocoding API keys.
	$form['location'] = array(
    '#type' 		   => 'fieldset',
    '#title' 		   => t('Location API Keys'),
    '#collapsible' => FALSE,
    '#collapsed' 	 => FALSE,
    '#weight'		   => -100,
    '#group'       => 'location',
	);
  $form['location']['phtools_map_key_google_geocoder'] = array(
    '#type'          => 'textfield',
    '#title' 		     => t('Google Map API Key'),
    '#description' 	 => t('Please enter your google api key here. While this is not required, you cannot use the phTools::_geoCode functionality without it.'),
    '#default_value' => variable_get('phtools_map_key_google_geocoder', NULL),
    '#required'      => TRUE,
    '#weight'		     => -100,
    '#group'         => 'location',
  );
  $form['location']['phtools_map_key_yahoo_geocoder'] = array(
    '#type'          => 'textfield',
    '#title' 		     => t('Yahoo Map API Key'),
    '#description' 	 => t('Please enter the Yahoo API key to allow the Yahoo API to geocode your query. It is recommended that if you enter a google map api key, that you also enter a Yahoo API key, in case google denies you request, or cannot find the location.'),
    '#default_value' => variable_get('phtools_map_key_yahoo_geocoder', NULL),
    '#required'      => FALSE,
    '#weight'		     => -100,
    '#group'         => 'location',
  );
  //Add our own validate handler.
  //@todo - if we need it - $form['#validate'][]   = '_ph_tools_location_validate';
  //Render vertical tabs.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  return system_settings_form($form);
}