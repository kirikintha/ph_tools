<?php

/**
 * @name _custom_enhancements_main()
 * @todo turn this into a a theme
 */
function _custom_enhancements_main() {
	
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
		'customEnhancements::_strip_views_keys( $str ) - this removes many of the common keys that make for long views lines. Use this to trim your views into manageable pieces.',
		'customEnhancements::_browser_check() - this function returns the current browser, and is available to all modules. returns an array $browser[\'version\'] && $browser[\'type\']',
		'customEnhancements::_list_months() - this function lists all Julian Calendar Months in a nn format, for use in your select boxes!',
		'customEnhancements::_list_years( $startDate, $endDate ) - this function returns the years you specify. ex: return $options = listYears( date( "Y", strtotime("- 3 years") ), date( "Y", strtotime("+3 years") ) );',
		'customEnhancements::_list_countries_iso() - this returns all countries as of 2009. Returns an array, please see the module for the array returned.',
		'customEnhancements::_list_countries_iso_cck() - this returns all countries as of 2009. Returns an array that is easily useable in CCK select fields.',
    'customEnhancements::_list_us_states() - returns current US states as of 2009. This array is compatible with CCK, just return the value from this array.',
		'customEnhancements::_list_canadian_provinces() - returns current Canadian Provinces as of 2009. This array is compatible with CCK, just return the value from this array.',
    'customEnhancements::_get_season( $to ) - returns an array (\'name\',\'date\') and allows you to get the current season ex: customEnhancements::_get_season() or a season in the future customEnhancements::_get_season( 180 )'
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
	
	//Jquery Plugins
	$items = array(
		'Jquery Cookies - lets you easily manage cookies. http://www.electrictoolbox.com/jquery-cookies/ ',
		'Jquery Cycle - allows you to cycle through just about anything. http://malsup.com/jquery_plugins/cycle/',
		'J Carousel - makes for easy image galleries. http://sorgalla.com/jcarousel/',
		'Masked Input - allows you to mask the input of your forms. http://digitalbush.com/projects/masked-input-plugin/',
    'Shadowbox - a web-based media viewer application http://www.shadowbox-js.com/usage.html',
		'Orbital Tooltip - one of many tooltips, a little buggy but good. http://www.userfirstinteractive.com/2008/12/01/orbital-tooltip/',
		'Preload CSS Images, preload your CSS url() images. http://www.filamentgroup.com/lab/update_automatically_preload_images_from_css_with_jquery/',
  	'PNG Fix, fix PNG transparency in IE 5 && 6. http://jquery.andreaseberhard.de/pngFix/'
	);
	$output .= theme('item_list',$items,'Jquery Plugins','ul');
	
	//Output everything to the markup field
	$form['main'] = array(
    '#type'   => 'markup',
    '#value'  => $output,
    '#weight' => 0,
	);
	
	return $form;

}

//Customizeable Elements
//@todo make this an array that loops over the $form array?
function _custom_enhancements_customize() {
	
	//Jquery Library options
	$form['jquery_plugins'] = array(
		'#type' 				=> 'fieldset',
		'#title' 				=> t('Jquery Plug-ins'),
		'#collapsible' 	=> FALSE,
		'#collapsed' 		=> FALSE,
		'#weight'				=> -20,
    '#group'        => 'jquery_plugins'
	);
  
	$output = '<p>Jquery plugins are plugins that can be enabled globally. All plug-ins are enabled by default</p>';
	$form['jquery_plugins']['info'] = array(
		'#type' 	=> 'markup',
		'#value' 	=> $output,
		'#weight' => 0,
    '#group'  => 'jquery_plugins'
	);
  
	$options = array('DISABLE','ENABLE');
	$form['jquery_plugins']['custom_enhancements_enable_jquery_carousel'] = array(
	  '#title' 					=> 'Enable or disable Jquery Carousel',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_carousel', '') ? variable_get('custom_enhancements_enable_jquery_carousel', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 1,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_cycle'] = array(
	  '#title' 					=> 'Enable or disable Jquery Cycle',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_cycle', '') ? variable_get('custom_enhancements_enable_jquery_cycle', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 2,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_maskedinput'] = array(
	  '#title' 					=> 'Enable or disable Jquery Masked Input',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_maskedinput', '') ? variable_get('custom_enhancements_enable_jquery_maskedinput', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 3,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_shadowbox'] = array(
	  '#title' 					=> 'Enable or disable Shadowbox',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_shadowbox', '') ? variable_get('custom_enhancements_enable_jquery_shadowbox', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 4,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_niceforms'] = array(
	  '#title' 					=> 'Enable or disable jqTransform',
	  '#description' 		=> 'jqTransform is a theme override system, that takes control over Drupal\'s form elements.',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_niceforms', '') ? variable_get('custom_enhancements_enable_jquery_niceforms', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 5,
    '#group'          => 'jquery_plugins'
	);
  
  $css = variable_get( 'custom_enhancements_enable_jquery_niceforms_css', CUSTOM_ENHANCEMENTS_MODULE_PATH. '/jquery_plugins/jqtransform/jqtransform.css' );
  $form['jquery_plugins']['custom_enhancements_enable_jquery_niceforms_css'] = array(
	  '#title' 					=> 'jqTransform CSS File Locations',
	  '#description' 		=> 'If you wish to change the location in which Nice Forms styles the forms, change it here.',
	  '#type' 					=> 'textfield', 
	  '#default_value' 	=> $css,
    '#weight'         => 6,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_orbitaltooltip'] = array(
	  '#title' 					=> 'Enable or disable Jquery Orbital Tooltip',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_orbitaltooltip', '') ? variable_get('custom_enhancements_enable_jquery_orbitaltooltip', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 7,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_preloadimages'] = array(
	  '#title' 					=> 'Enable or disable Jquery Preload Images from CSS',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_preloadimages', '') ? variable_get('custom_enhancements_enable_jquery_preloadimages', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 8,
    '#group'          => 'jquery_plugins'
	);
  
	$form['jquery_plugins']['custom_enhancements_enable_jquery_pngfix'] = array(
	  '#title' 					=> 'Enable or disable Jquery PNG Fix',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('custom_enhancements_enable_jquery_pngfix', '') ? variable_get('custom_enhancements_enable_jquery_pngfix', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 9,
    '#group'          => 'jquery_plugins'
	);

	//Fade messages
	$form['fade_messages'] = array(
		'#type' 				=> 'fieldset',
		'#title' 				=> t('Fade Out Messages'),
		'#collapsible' 	=> FALSE,
		'#collapsed' 		=> FALSE,
		'#weight'				=> -19,
    '#group'          => 'fade_messages'
	);
  
	$output = '<p>Drupal status, warning and error messages stick to the page. This allows you to set which messages fade, and on which urls</p>';
	$form['fade_messages']['info'] = array(
		'#type' 	=> 'markup',
		'#value' 	=> $output,
		'#weight' => 0,
    '#group'  => 'fade_messages'
	);
  
	$options = array('DISABLE','ENABLE');
	$form['fade_messages']['fade_messages_override'] = array(
	  '#title' 					=> 'Enable or disable message fading',
	  '#description' 		=> '',
	  '#type' 					=> 'radios', 
	  '#default_value' 	=> variable_get('fade_messages_override', '') ? variable_get('fade_messages_override', '') : 0,
	  '#options' 				=> $options,
    '#weight'         => 1,
    '#group'          => 'fade_messages'
	);
  
	$form['fade_messages']['custom_enhancements_classes_to_fade'] = array(
	  '#title' 					=> 'Classes to fade',
	  '#description' 		=> 'Enter the class names of the classes of the messages you wish to fade.',
	  '#type' 					=> 'textarea', 
	  '#default_value' 	=> variable_get('custom_enhancements_classes_to_fade', "status\nwarning" ),
    '#weight'         => 2,
    '#group'          => 'fade_messages'
	);
  
	$form['fade_messages']['custom_enhancements_fade_out_paths'] = array(
	  '#title' 					=> 'Paths to exclude from fading',
	  '#description' 		=> 'Enter the paths you do NOT want to fade, like the admin section.',
	  '#type' 					=> 'textarea', 
	  '#default_value' 	=> variable_get('custom_enhancements_fade_out_paths', "admin*\n"),
    '#weight'         => 3,
    '#group'          => 'fade_messages'
	);
  
	$form['fade_messages']['custom_enhancements_fade_out_delay'] = array(
	  '#title' 					=> 'Timeout before fade happens',
	  '#description' 		=> 'Enter the timeout value, in miliseconds before the timeout fires the fade out',
	  '#type' 					=> 'textfield', 
	  '#default_value' 	=> variable_get('custom_enhancements_fade_out_delay',5500),
    '#weight'         => 4,
    '#group'          => 'fade_messages'
	);
  
	//Strip Keys
	$form['views_strip_keys'] = array(
		'#type' 				=> 'fieldset',
		'#title' 				=> t('Views Strip Keys'),
		'#collapsible' 	=> TRUE,
		'#collapsed' 		=> TRUE,
		'#weight'				=> -18,
    '#group'        => 'views_strip_keys'
	);
  
	$output = '<p>Strip out Views Keyes allows you to trim down views keys to a reasonable key.</p>';
	$form['views_strip_keys']['info'] = array(
		'#type' 	=> 'markup',
		'#value' 	=> $output,
		'#weight' => 0,
    '#group'  => 'views_strip_keys'
	);
  
	$form['views_strip_keys']['custom_enhancements_views_strip_keys_keys'] = array(
	  '#title' 					=> 'Keywords to strip',
	  '#description' 		=> 'Enter in keywords to srtip, this will strip out everything! Spearate by a semi-colon',
	  '#type' 					=> 'textarea', 
	  '#default_value' 	=> variable_get('custom_enhancements_views_strip_keys_keys', "" ),
    '#weight'         => 1,
    '#group'          => 'views_strip_keys'
	);
  
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render'; //enabled vertical tabs if the module is installed
	
  return system_settings_form($form);

}