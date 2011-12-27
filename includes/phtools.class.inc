<?php
/**
 * @name Custom Enhancements
 * A set of re-useable functions:
 * @static _strip_views_keys($str)
 * @static _set_header_nocache()
 * @static _browser_check()
 * @static _list_months()
 * @static _list_years($startDate, $endDate)
 * @static _list_canadian_provinces()
 * @static _list_us_states()
 * @static _list_countries_iso()
 * @static _list_countries_iso_cck()
 * @static _get_season($to = 0)
 */

class phTools {

  /**
   * @name _interceptPage($theme_info = FALSE)
   */
  public static function _interceptPage($vars, $theme_info = FALSE) {
    //If we have a valid theme object, delegate out our content changes.
    if (is_object($theme_info)) {
      //Get the theme settings for this theme.
      $theme_settings = theme_get_settings($theme_info->name);
      if (is_array($theme_settings)) {
        //Set Meta Information.
        $vars = self::_interceptMeta($vars, $theme_settings, $theme_info);
        //Set Local Stylesheet.
        $vars = self::_interceptStyles($vars, $theme_settings, $theme_info);
        //Intercept the javascript to disallow any non-essential javascript to the page.
        $vars = self::_interceptJS($vars, $theme_settings, $theme_info);
      }
      //Intercept body id and body classes and add appropriate $vars
      //$vars = self::_interceptBody($vars, $theme_settings, $theme_info);
      //After we process the page, remove the theme settings array.
      unset($theme_settings, $theme_info);
    }
    //Get body id and additional classes.
    $vars = self::_interceptBody($vars, $theme_settings, $theme_info);
    //Return our changed variables.
    return $vars;
  }

  /**
   * @name _interceptMeta(&$vars)
   */
  public static function _interceptMeta($vars, $theme_settings = NULL, $theme_info = FALSE) {
    //Get our theme settings and use the meta tags in here.
    $vars['intercept_head'] = $vars['head'];
    $meta = $theme_settings['intercept_meta_tags'];
    $tags = array();
    if (count($meta) > 0 ) {
      foreach ($meta as $tag) {
        $tags[] = '<meta name="'.trim($tag['meta_name']).'" content="'.trim($tag['meta_content']).'" />';
      }
      $vars['intercept_head'] .= implode("\n", $tags) ."\n";
    }
    //Return $vars
    return $vars;
  }
  
  /**
   * @name _interceptStyles($vars, $theme_settings)
   * Get this theme's stylesheet.
   * @todo - Need to suss out how to get the styles to have the admin_menu css and js added back in as well intelligently.
   */
  public static function _interceptStyles($vars, $theme_settings = NULL, $theme_info = FALSE) {
    /**
     * IE Stylesheets.
     */
    $path = drupal_get_path('theme', $theme_info->name);
    $link = '<link type="text/css" rel="stylesheet" media="all" href="' .base_path() .$path;
    $vars['ie_styles']  = (file_exists($path . '/css/styles.ie.css')) ? '<!--[if IE]>' .$link . '/css/styles.ie.css" /><![endif]-->' . "\n" : '';
    $vars['ie7_styles'] = (file_exists($path . '/css/styles.ie7.css')) ? '<!--[if IE 7]>' .$link . '/css/styles.ie7.css" /><![endif]-->' . "\n" : '';
    $vars['ie8_styles'] = (file_exists($path . '/css/styles.ie8.css')) ? '<!--[if IE 8]>' .$link . '/css/styles.ie8.css" /><![endif]-->' . "\n" : '';
				$vars['ie9_styles'] = (file_exists($path . '/css/styles.ie9.css')) ? '<!--[if IE 9]>' .$link . '/css/styles.ie9.css" /><![endif]-->' . "\n" : '';
    //If we are introducing our own aggregated stylesheets into the theme, then use this parser and aggregator.
    if ($theme_settings['intercept_styles'] == 1) {
      //Aggregate stylesheets.
      $styles = $vars['css']['all'];
      //Look through our stylesheets to see if they are allowed.
      if (!empty($styles)) {
        //The scripts array has module, theme as keys.
        $types = array('module', 'theme');
        //Loop through each type we want to constrain and unset those variables.
        $files = $no_cache = array();
        foreach ($types as $type) {
          foreach ($styles[$type] as $key => $info) {
            if (!empty($theme_settings['intercept_styles_excludes'])) {
              if (in_array($key, $theme_settings['intercept_styles_excludes'])) {
                //If our script coming through is in the exclusion list, then we need to unset the css we do not want.
                unset($styles[$type][$key]);
              } else {
                //Set up our stylesheets files for aggregation.
                if ($info) {
                  $files[$type][$key] = $info;
                } else {
                  $no_cache[] = $key;
                }
              }
            } else {
              //Set up our stylesheets files for aggregation.
              if ($info) {
                $files[$type][$key] = $info;
              } else {
                $no_cache[] = $key;
              }
              unset($styles[$type][$key]);
            }
          }
        }
        //The only thing I want this to do is aggregate.
        if (count($files) > 0) {
          $filename = 'css_custom_'.  md5(serialize($files)).'.css';
          $file     = drupal_build_css_cache($files, $filename);
        }
        //Return our aggregated styles.
        $vars['intercept_styles'] = '<link type="text/css" rel="stylesheet" media="all" href="' .$file. '" />';
        foreach ($no_cache as $file) {
          $vars['intercept_styles'] .= '<link type="text/css" rel="stylesheet" media="all" href="' .$file. '" />';
        }
      }
    } else {
      //If we do not have any overrides, then just return the regular scripts to the page.
      $vars['intercept_styles'] = $vars['styles'];
    }
    return $vars;
  }

  /**
   * @name _interceptJS(&$vars)
   * Intercepts $vars['scripts'] and turns it into $['intercept_scripts'], removes unwanted JS and aggregates it into it's own separate js file.
   */
  public static function _interceptJS($vars, $theme_settings = NULL, $theme_info = FALSE) {
    $scripts = drupal_add_js(NULL, NULL, NULL);
    //Since jquery update is a little wonky on when it replaces jquery, then we need to do the replacements here, since for some reason the page_process can fail.
    //Since this is taken straight from the jquery update module, if there are changes or whatever - this has to be updated too.
    if (variable_get('jquery_update_replace', TRUE)||
        variable_get('jquery_update_compression_type', 'pack') != 'pack') {
      // Replace jquery.js first.
      $new_jquery = array(jquery_update_jquery_path() => $scripts['header']['core']['misc/jquery.js']);
      $scripts['header']['core'] = array_merge($new_jquery, $scripts['header']['core']);
      unset($scripts['header']['core']['misc/jquery.js']);

       // Loop through each of the required replacements.
      foreach (jquery_update_get_replacements() as $type => $replacements) {
        foreach ($replacements as $find => $replace) {
          // Create a new entry for the replacement file, and unset the original one.
          $replace = JQUERY_UPDATE_REPLACE_PATH .'/'. $replace;
          if (isset($scripts['header'][$type][$find]) && !empty($scripts['header'][$type][$find])) {
            $scripts['header'][$type][$replace] = $scripts['header'][$type][$find];
            unset($scripts['header'][$type][$find]);
          }
          if (isset($scripts['footer'][$type][$find]) && !empty($scripts['footer'][$type][$find])) {
            $scripts['footer'][$type][$replace] = $scripts['footer'][$type][$find];
            unset($scripts['footer'][$type][$find]);
          }
          if (isset($scripts[$type][$find]) && !empty($scripts[$type][$find])) {
            $scripts[$type][$replace] = $scripts[$type][$find];
            unset($scripts[$type][$find]);
          }
        }
      }
    }
    //If we are introducing our own aggregated js to the theme, then use this parser and aggregator.
    if ($theme_settings['intercept_js'] == 1) {
      //Restrict javascript to the essentials only!
      //Look through our scripts to see if they are allowed.
      if (!empty($scripts['header'])) {
        //The scripts array has core, module, theme, settings etc as keys.
        $types = array('module');
        //Loop through each type we want to constrain and unset those variables.
        $files = array();
        foreach ($types as $type) {
          foreach ($scripts['header'][$type] as $key => $info) {
            if (in_array($key, $theme_settings['intercept_js_excludes'])) {
              //If our script coming through is in the exclusion list, then we need to unset the js we do not want.
              unset($scripts['header'][$type][$key]);
            } else {
              //Set up our js files for aggregation.
              if ($info['preprocess']) {
                $files[$key] = $info;
                unset($scripts['header'][$type][$key]);
              }
            }
          }
        }
        //The only thing I want this to do is aggregate.
        if (count($files) > 0) {
          $filename        = 'js_custom_'.  md5(serialize($files)).'.js';
          $preprocess_file = drupal_build_js_cache($files, $filename);
          $info = array(
            'cache'      => TRUE,
            'defer'      => FALSE,
            'preprocess' => FALSE,
          );
          $scripts['header']['theme'][$preprocess_file] = $info;
        }
        //Return our scripts.
        $vars['intercept_scripts'] = drupal_get_js('header', $scripts['header']);
      }
    } else {
      //If we do not have any overrides, then just return the regular scripts to the page.
      $vars['intercept_scripts'] = drupal_get_js('header', $scripts['header']);
    }
    return $vars;
  }

  /**
   * @name _interceptBody($vars, $theme_settings = NULL)
   * @param <type> $vars
   * @param <type> $theme_settings
   * @toto - something about RDFa can go here I bet.
   */
  public static function _interceptBody($vars, $theme_settings = NULL, $theme_info = FALSE) {
    //Add Body ID to each page to keep pages unique.
    $vars['body_id'] = 'pid-' . strtolower(preg_replace('/[^a-zA-Z0-9-]+/', '-', drupal_get_path_alias($_GET['q'])));
    //Add in classes that are more robust than drupal's
    $vars = self::_getBodyClasses($vars, $theme_settings = NULL, $theme_info = FALSE);
    return $vars;
  }

  /**
   * Body Class Adjustments
   * @internal this includes body classes for property reviews and property maps, these have to be hardcoded, because there is no way to get them from views the way I have it
   */
  public static function _getBodyClasses($vars, $theme_settings = NULL, $theme_info = FALSE) {
    $bodyClasses = array();
    $sysadmin = FALSE;
    //Add in the node-% before anything else, so that the node admin overrides anything else when we apply classes.
    //Node Add Class
    if (arg(0) == 'node' && arg(1) == 'add') {
      $bodyClasses['node-add'] = 'node-add';
      $sysadmin = TRUE;
    }
    //Node View class, only when we are really viewing a node!
    if (arg(0) == 'node' && is_numeric(arg(1)) && count(arg()) == 2) {
      $bodyClasses['node-view'] = 'node-view';
    }
    //Node Edit Class
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'edit') {
      $bodyClasses['node-edit'] = 'node-edit';
      $sysadmin = TRUE;
    }
    //Node Edit Class when editing webform components
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'edit' && arg(3) == 'components') {
      $bodyClasses['node-webform-components'] = 'node-webform-components';
    }
    //Node Edit Class when editing node revisions
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'revisions' && is_numeric(arg(3)) && arg(4) == 'edit') {
      $bodyClasses['node-edit'] = 'node-edit';
      $sysadmin = TRUE;
    }
    //Node Revisions Class when viewing node revisions
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'revisions') {
      $bodyClasses['node-revisions'] = 'node-revisions';
      $sysadmin = TRUE;
    }
    //Page classes for URI, so we can have an alias to work with in the body class only top level, arg(0) like aliases, this is not arg() though
    if (drupal_get_path_alias(request_uri())) {
      $uri = explode("/", substr(drupal_get_path_alias(request_uri()), 1));
      $bodyClasses['uri'] = strtolower($uri[0]);
    }
    //If the sysadmin flag is set, return a sys-admin class
    if (arg(0) ==  'admin' || $sysadmin === TRUE) {
      $bodyClasses['system-admin'] = 'system-admin';
    }
    //Internal Drupal CSS.
    //Turn $inputs into the body classes - this is what the system is giving us!
    $classes = explode(' ', $vars['body_classes']);
    foreach ($classes as $input) {
      $bodyClasses[$input] = check_plain($input); //turn the body classes into a keyed array so we can unset them if we want
    }
    //Sidebar regions - I have no clue why these won't populatem but they don't;
    if (!empty($vars['sidebar_first']) && !isset($bodyClasses['sidebar-last'])) {
      $bodyClasses['sidebar-first'] = 'sidebar-first';
    }
    if (!empty($vars['sidebar_last']) && !isset($bodyClasses['sidebar-last'])) {
      $bodyClasses['sidebar-last'] = 'sidebar-last';
    }
    if (isset($bodyClasses['sidebar_first']) || isset($bodyClasses['sidebar-last'])) {
      unset($bodyClasses['no-sidebars']);
    }
    //Output classes.
    $output = implode(' ', $bodyClasses);
    $vars['body_classes'] = $output; // Create class list separated by spaces
    return $vars;
  }

  public static function _getBreadcrumbs($breadcrumb = NULL){
    //Title from page.
    $lastArg  = count(arg()) - 1; //The last argument is always one less
    $title    = empty($node) ? drupal_get_title() : $node->title; //if there is no node loaded, then use the page title as a backup
    $title    = (!menu_get_active_title() && empty($node)) ? $title : menu_get_active_title(); //Get the menu title if there is one
    $title    = ($title == '') ? ucwords(str_replace("-", " ", arg($lastArg))) : $title; //If there is still no title, take the last argument and use that as the title
    if (!empty($title)) { //If there is no title at all, do not process the title
      //If we are in search results, then use 'results' as a search option.
      if (arg(0) == 'search' && count(arg()) > 1) {$title = 'Results';}
    }
    //Core breadcrumbs.
    $breadcrumbs = array(); //We want to give all the breadcrumbs a meaningful key, that is not a number, so if we are adding to the key, it does not already exist
    foreach($breadcrumb as $key => $value) { //Preg Match the url's already coming through, and do not add a breadcrumb for the url's already made
      preg_match('/href="([^"]*)"/i', $value , $matches);
      $newKey                = str_replace("/", "", $matches[1]); //Home does not produce a match, so we need to get that in here no matter what
      $breadcrumbs[$newKey]  = $value;
    }
    $path      = implode("/", arg()); //Implode the arg into a path, this is better than messing with the URI Request
    $pathAlias = drupal_get_path_alias($path); //Get the path alias from the path, so that anything can be used as a breadcrumb
    $node = (arg(0) == 'node') ? node_load(arg(1)) : '' ; //Load a node from the system path, return an emtpy array, this will guarantee it is assigned properly
    $pathAliasArr  = explode("/", $pathAlias); //Explode the path alias into an array
    $pathAliasKeys = array_keys($pathAliasArr); //Get the Last key, so we never render that
    $lastPathKey   = end($pathAliasKeys);
    if (is_array($pathAliasArr)) { //If the path is an array, which means it has a depth of more than zero, then render the path out as links
      $trail = array();
      foreach($pathAliasArr as $key => $value) { //render keys as arrays, checking if the path exists or not
        $trail[] = $value;
        $href = implode('/',$trail);
        $key = str_replace("/", "", $href);
        $alias_title = db_result(db_query("SELECT link_title FROM {url_alias} u INNER JOIN {menu_links} ml ON u.src = ml.link_path WHERE dst = '%s'", $href));
        if ($alias_title) {
          $link['@title'] = $alias_title;
        } else {
          //If we do not have an alias title, then look for a regular menu title from menu links.
          $link_title = db_result(db_query("SELECT link_title FROM {menu_links} WHERE link_path = '%s' AND menu_name != 'navigation'", $href));
          if ($link_title) {
            //If we have a link title, then use that.
            $link['@title'] = $link_title;
          } else {
            //If we have nothing else for this path, then resort to the value.
            $link['@title'] = ucwords($value);
          }
        }
        //If we have an alias title, use that.
        $breadcrumbs[$key] = l(t('@title', $link), $href);
      }
    }
    //Take off last breadcrumb, since this is the active crumb.
    array_pop($breadcrumbs);
    $breadcrumbs["last"] = '<span class="last-breadcrumb">'. t($title) .'</span>';
    if (!drupal_is_front_page()) {
      return '<div class="breadcrumbs">'. implode(' > ', $breadcrumbs) .'</div>';
    }
  }

  /**
   * @name _getSearchResultsFieldset()
   * This makes a fieldset for you that have the proper exposed filter form
   * @param strings $name, $display The views display parameters
   * @param bool $asCheckboxes
   * @return string the rendered form
   */
  public static function _getSearchResultsFieldset($name = NULL, $display = NULL, $asCheckboxes = FALSE) {

    $form['general_information'] = array(
      '#type' 				=> 'fieldset',
      '#title' 				=> t('Refine Your Search'),
      '#collapsible' 	=> TRUE,
      '#collapsed' 		=> TRUE,
      '#weight'				=> -50
   );

    $data = self::_getExposedFilters($name, $display, $asCheckboxes);

    $form['general_information']['info'] = array(
      '#value' => $data
   );

    return drupal_render_form('general_information', $form);

  }

  /**
   * Get Exposed Filters
   */
  public static function _getExposedFilters($name, $display, $asCheckboxes) {

    $view = views_get_view($name);

    if ($view) {

      $view->set_display($display);
      $view->init_handlers();

      $form_state = array(//Form State Array
        'view'          => $view,
        'display'       => $view->display_handler->display,
        'method'        => 'get',
        'rerender'      => TRUE,
        'no_redirect'   => TRUE,
        'asCheckboxes'  => $asCheckboxes
     );

      $output = drupal_build_form('views_exposed_form', $form_state);

      return $output;

    }

  }

  /**
   * @name _getSearchDestination()
   * @param array $vars = the variables from the template proprocess function
   * @return the destination you need to point to.
   */
  public static function _getSearchDestination($vars) {
    $dom = new domDocument;
    @$dom->loadHTML($vars['output']);
    $dom->preserveWhiteSpace = false;
    $links = $dom->getElementsByTagName('a');
    foreach ($links as $tag) {$href = $tag->getAttribute('href');}
    return str_replace($href, $href .'?' .drupal_get_destination(), $vars['output']);
  }

  /**
   * _geoCode($address)
   *  Geocode an address using the address object.
   * @param type $address
   *  $address->street_line1;
   *  $address->street_line2;
   *  $address->city;
   *  $address->province;
   *  $address->postal_code;
   * @return type 
   */
  public static function _geoCode($address) {
    $goecode = array();
    //Reset status code returned from google.
    $status = NULL;
    //Check to see if we need to use google.
    $disableGoogle = variable_get('phtools_disable_google_geocoding', FALSE);
    //We need to get two keys from google and yahoo - so if one fails, we can use the other.
    $googleKey = variable_get('phtools_map_key_google_geocoder', null);
    $yahooKey  = variable_get('phtools_map_key_yahoo_geocoder', null);
    //Use the location BTW - this is compatible with the location module too.
    $location['location_street']   = array();
    $location['location_street'][] = $address->street_line1;
    $location['location_street'][] = $address->street_line2;
    //Translate location to something google geocode can understand.
    $street      = implode(' ', $location['location_street']);
    $city        = $address->city;
    $province    = $address->state;
    $postal_code = $address->zip;
    $country     = $address->country; //@internal - I don't know if this is needed?
    //If we can parse through google.
    if (!$disableGoogle && !empty($googleKey)) {
      //Parse the data and get a response
      $data  = urlencode("$street $city, $province, $postal_code");
      $q     = $data;
      $url   = 'http://maps.google.com/maps/geo?output=xml&key='. $key .'&q=' .$q;
      $reply = drupal_http_request($url);
      //kpr($reply);
      $xml   = self::_validateData($reply->data);
      if ($xml) {
        //Get the status back from google.
        $status = (int) $xml->Response->Status->code;
        if ($status == 620) {
          variable_set('phtools_disable_google_geocoding', time()); //Set this to disable for 24 hours, and use Yahoo geocoding then
        } else {
          //Get Geocoded data back.
          $geocodeData = (string) $xml->Response->Placemark->Point->coordinates; //Since we get a valid XML string back from validate, we can just traverse the tree
          $geocodeData = explode(',', $geocodeData); //explode on the comma
          //Turn google info into lat & long data
          $geocode['lon'] = $geocodeData[0]; //position 0 is the longitude
          $geocode['lat'] = $geocodeData[1]; //position 1 is the latitude
        }
      }
    }
    //Fall Back to using Yahoo's geocoding service, this guarantees we can have upto 15,0000 calls a day between the two
    if (($disableGoogle || !is_array($geocodeData)) && !empty($yahooKey)) {
      $key   = variable_get('phtools_map_key_yahoo_geocoder', null);
      $url   = 'http://local.yahooapis.com/MapsService/V1/geocode?appid='. $key .'&street=' .urlencode($street) .'&city=' .urlencode($city) .'&state=' .urlencode($province) .'&zip=' .urlencode($postal_code);
      $reply = drupal_http_request($url);
      //print_r($reply);
      $xml   = self::_validateData($reply->data);
      if ($xml) {
        $geocode['lat'] = (string) $xml->Result->Latitude;
        $geocode['lon'] = (string) $xml->Result->Longitude; 
      }
      //@todo - this has to ignore google, if google decides to be a PITA.
    }
    //Sleep the geocode, when geocoding - to make sure that google does not report a 620 limit error. They will ban you for 24hours if you don't.
    //This is to combat bulk operations, if we ever run bulk operations through this method.
    usleep(100000);
    //Return the array back to the location module.
    return $geocode;
  }

  /**
   * @name createMetaData($node = NULL)
   * @param <type> $node
   */
  public static function _createMetaData($node = NULL) {
    if (is_object($node) && isset($node->nodewords)) {
      //CCK Field Body will overtake the body tag here - choose one or the other, not the body
      if (isset($node->body) && isset($node->field_body[0]['value'])) {
        //Remove stop words, as defined by the path alias module.
        $stopwords_list = self::_getStopWords();
        //Get words from field_body and check_plain.
        $words = check_plain(strip_tags($node->field_body[0]['value']));
        //Sanitize keywords.
        $words = self::_stripPunctuation($words);
        $words = self::_stripSymbols($words);
        $words = self::_stripNumbers($words);
        //Split into array.
        $words = explode(' ', $words);
        //Get the array count of the words, so we can know how many times they are repeated.
        $words_count = array_count_values($words);
        //Reverse the sort, so higher number words come first for output.
        arsort($words_count);
        $keywords = array();
        foreach ($words_count as $word => $count) {
          $word = (string) trim(strtolower($word));
          $keywords[$word] = $word;
        }
        //Get the total words, so we can analyze the density.
        $keywords_total = count($keywords);
        //Create keyword list.
        $keywords_list = array();
        foreach ($keywords as $key => $word) {
          $word = (string) trim(strtolower($word));
          //Validate a keyword, by the following criteria:
          //  1. If the word is more than two letters long. @internal - validating letters may need to become 3 or more, but keywords like CCK and PHP can be important.
          //  2. If the word is strictly alpha characters. @internal - this really just to save headaches and any unwanted character groupings from coming through.
          //  3. If the word is not on the stopwords list.
          //  4. If the keyword density is between 1% and 3%. @internal - keywords are considered a "legacy" technique by modern standards,
          //     but I still believe that relevent meta keywords are important to your page.
          //  The cumulative effect here is to only populate a few relevent keywords that are used enough. To promote keywords, you need to
          //  re-use those keywords thoughtfully in your writing, which will be a blog post.

          //Validate keyword.
          if (strlen($word) > 2) {
            if (preg_match('/^[a-zA-Z]+$/', $word) && !in_array($word, $stopwords_list)) {
              //Keyword Density Score.
              $keyword_density = ($words_count[$word]/$keywords_total) * 100;
              if ($keyword_density > 1 && $keyword_density < 3) {
                $keywords_list[$word] = $word;
              }
            }
          }
        }
        //Keywords list becomes nodewords keywords and field_body trimmed becomes description.
        $keywords    = implode(', ', $keywords_list);
        //Node words will only let you have 350 characters for keywords, so we will just just off anything that is over the default limit of 350.
        //Keywords really should only be 120 character, so this is a lot.
        //Since we are keeping the most frequently used keywords first, if there are too many, than only lower density keywords drop off.
        $keywords = self::_wordTrim($keywords, variable_get('nodewords_max_size', 120), 'char');
        //Description.
        $description = self::_wordTrim($node->field_body[0]['value'], 40);
        //Set the nodewords values and save.
        $MSG['!copy'] = '&copy;';
        $MSG['@date'] = date('Y', $node->created);
        $MSG['@company'] = variable_get('site_name', NULL);
        //Abstract
        $node->nodewords['abstract']['value'] = self::_wordTrim($description, 12);
        //Canonical URL.;
        $node->nodewords['canonical']['value'] = $node->path;
        //Set copyright if empty.
        if (empty($node->nodewords['copyright']['value'])) {
          $node->nodewords['copyright']['value'] = t('!copy @date @company', $MSG);
        }
        //Keywords
        $node->nodewords['keywords']['value'] = $keywords;
        //Description
        $node->nodewords['description']['value'] = check_plain($description);
        //Dublin Core Meta information.
        $node->nodewords['dc.contributor']['value'] = t('@company', $MSG);
        $node->nodewords['dc.creator']['value']     = check_plain($node->name);
        $node->nodewords['dc.date']['value']        = array(
          'month' => date('n', $node->created),
          'day' => date('j', $node->created),
          'year' => date('Y', $node->created),
        );
        $node->nodewords['dc.title']['value']       = check_plain($node->title);
        //Prepare Node and save.
        $node = node_prepare($node);
        node_save($node);
      }
    }
  }
  
  /**
   *
   * @return <type>
   */
  public static function _getStopWords() {
    //include stopwords from phtools.stopwords.inc
    require_once(PH_TOOLS_MODULE_PATH. '/includes/phtools.stopwords.inc');
    $words = phtoolsStopWords::_getStopWordsList();
    $words = explode(',', $words);
    $stopwords_list = array();
    foreach ($words as $key => $word) {
      $word = (string) trim(strtolower($word));
      $search = array(
        "\n",
        "\r",
        "\t",
      );
      $replace = array(
        "",
        "",
        "",
      );
      $word = str_replace($search, $replace, $word);
      $stopwords_list[$word] = $word;
    }
    return $stopwords_list;
  }

 /**
  * Trim a string to a given number of words
  *
  * @param $string
  *   the original string
  * @param $count
  *   the word count
  * @param $ellipsis
  *   TRUE to add "..."
  *   or use a string to define other character
  * @param $op
  *   Either trim by word count, or string count and preserve word boundary.
  *
  * @return
  *   trimmed string with ellipsis added if it was truncated
  */
  public static function _wordTrim($string = NULL, $count = 120, $op = 'word', $ellipsis = FALSE) {
    $words  = explode(' ', $string);
    $string = '';
    //Switch between ops
    switch ($op) {
      //Trim by word count.
      case 'word':
          if (count($words) > $count){
            array_splice($words, $count);
            $string = implode(' ', $words);
          }
        break;
      //Trim by character count, keep word boundary.
      case 'char':
          foreach ($words as $word) {
            if ((strlen($string) + strlen($word)) < $count) {
              $string .= $word .' ';
            }
          }
          $string = trim($string);
        break;
    }
    //Ellipsis
    if (is_string($ellipsis)){
      $string .= $ellipsis;
    } elseif ($ellipsis){
      $string .= '&hellip;';
    }
    return $string;
  }

  /**
   *
   * @param <type> $text
   * @return <type>
   */
  public static function _stripSymbols($text = NULL) {
    $plus   = '\+\x{FE62}\x{FF0B}\x{208A}\x{207A}';
    $minus  = '\x{2012}\x{208B}\x{207B}';

    $units  = '\\x{00B0}\x{2103}\x{2109}\\x{23CD}';
    $units .= '\\x{32CC}-\\x{32CE}';
    $units .= '\\x{3300}-\\x{3357}';
    $units .= '\\x{3371}-\\x{33DF}';
    $units .= '\\x{33FF}';

    $ideo   = '\\x{2E80}-\\x{2EF3}';
    $ideo  .= '\\x{2F00}-\\x{2FD5}';
    $ideo  .= '\\x{2FF0}-\\x{2FFB}';
    $ideo  .= '\\x{3037}-\\x{303F}';
    $ideo  .= '\\x{3190}-\\x{319F}';
    $ideo  .= '\\x{31C0}-\\x{31CF}';
    $ideo  .= '\\x{32C0}-\\x{32CB}';
    $ideo  .= '\\x{3358}-\\x{3370}';
    $ideo  .= '\\x{33E0}-\\x{33FE}';
    $ideo  .= '\\x{A490}-\\x{A4C6}';

    return preg_replace(
        array(
        // Remove modifier and private use symbols.
            '/[\p{Sk}\p{Co}]/u',
        // Remove mathematics symbols except + - = ~ and fraction slash
            '/\p{Sm}(?<![' . $plus . $minus . '=~\x{2044}])/u',
        // Remove + - if space before, no number or currency after
            '/((?<= )|^)[' . $plus . $minus . ']+((?![\p{N}\p{Sc}])|$)/u',
        // Remove = if space before
            '/((?<= )|^)=+/u',
        // Remove + - = ~ if space after
            '/[' . $plus . $minus . '=~]+((?= )|$)/u',
        // Remove other symbols except units and ideograph parts
            '/\p{So}(?<![' . $units . $ideo . '])/u',
        // Remove consecutive white space
            '/ +/',
        ),
        ' ',
        $text );
  }

  /**
   *
   * @param <type> $text
   * @return <type>
   */
  public static function _stripNumbers($text = NULL) {
    $urlchars      = '\.,:;\'=+\-_\*%@&\/\\\\?!#~\[\]\(\)';
    $notdelim      = '\p{L}\p{M}\p{N}\p{Pc}\p{Pd}' . $urlchars;
    $predelim      = '((?<=[^' . $notdelim . '])|^)';
    $postdelim     = '((?=[^'  . $notdelim . '])|$)';

    $fullstop      = '\x{002E}\x{FE52}\x{FF0E}';
    $comma         = '\x{002C}\x{FE50}\x{FF0C}';
    $arabsep       = '\x{066B}\x{066C}';
    $numseparators = $fullstop . $comma . $arabsep;
    $plus          = '\+\x{FE62}\x{FF0B}\x{208A}\x{207A}';
    $minus         = '\x{2212}\x{208B}\x{207B}\p{Pd}';
    $slash         = '[\/\x{2044}]';
    $colon         = ':\x{FE55}\x{FF1A}\x{2236}';
    $units         = '%\x{FF05}\x{FE64}\x{2030}\x{2031}';
    $units        .= '\x{00B0}\x{2103}\x{2109}\x{23CD}';
    $units        .= '\x{32CC}-\x{32CE}';
    $units        .= '\x{3300}-\x{3357}';
    $units        .= '\x{3371}-\x{33DF}';
    $units        .= '\x{33FF}';
    $percents      = '%\x{FE64}\x{FF05}\x{2030}\x{2031}';
    $ampm          = '([aApP][mM])';

    $digits        = '[\p{N}' . $numseparators . ']+';
    $sign          = '[' . $plus . $minus . ']?';
    $exponent      = '([eE]' . $sign . $digits . ')?';
    $prenum        = $sign . '[\p{Sc}#]?' . $sign;
    $postnum       = '([\p{Sc}' . $units . $percents . ']|' . $ampm . ')?';
    $number        = $prenum . $digits . $exponent . $postnum;
    $fraction      = $number . '(' . $slash . $number . ')?';
    $numpair       = $fraction . '([' . $minus . $colon . $fullstop . ']' .
        $fraction . ')*';

    return preg_replace(
        array(
        // Match delimited numbers
            '/' . $predelim . $numpair . $postdelim . '/u',
        // Match consecutive white space
            '/ +/u',
        ),
        ' ',
        $text );
  }

  /**
   *
   * @param <type> $text
   * @return <type>
   */
  public static function _stripPunctuation($text = NULL) {
    $urlbrackets    = '\[\]\(\)';
    $urlspacebefore = ':;\'_\*%@&?!' . $urlbrackets;
    $urlspaceafter  = '\.,:;\'\-_\*@&\/\\\\\?!#' . $urlbrackets;
    $urlall         = '\.,:;\'\-_\*%@&\/\\\\\?!#' . $urlbrackets;

    $specialquotes  = '\'"\*<>';

    $fullstop       = '\x{002E}\x{FE52}\x{FF0E}';
    $comma          = '\x{002C}\x{FE50}\x{FF0C}';
    $arabsep        = '\x{066B}\x{066C}';
    $numseparators  = $fullstop . $comma . $arabsep;

    $numbersign     = '\x{0023}\x{FE5F}\x{FF03}';
    $percent        = '\x{066A}\x{0025}\x{066A}\x{FE6A}\x{FF05}\x{2030}\x{2031}';
    $prime          = '\x{2032}\x{2033}\x{2034}\x{2057}';
    $nummodifiers   = $numbersign . $percent . $prime;

    return preg_replace(
        array(
        // Remove separator, control, formatting, surrogate,
        // open/close quotes.
            '/[\p{Z}\p{Cc}\p{Cf}\p{Cs}\p{Pi}\p{Pf}]/u',
        // Remove other punctuation except special cases
            '/\p{Po}(?<![' . $specialquotes .
                $numseparators . $urlall . $nummodifiers . '])/u',
        // Remove non-URL open/close brackets, except URL brackets.
            '/[\p{Ps}\p{Pe}](?<![' . $urlbrackets . '])/u',
        // Remove special quotes, dashes, connectors, number
        // separators, and URL characters followed by a space
            '/[' . $specialquotes . $numseparators . $urlspaceafter .
                '\p{Pd}\p{Pc}]+((?= )|$)/u',
        // Remove special quotes, connectors, and URL characters
        // preceded by a space
            '/((?<= )|^)[' . $specialquotes . $urlspacebefore . '\p{Pc}]+/u',
        // Remove dashes preceded by a space, but not followed by a number
            '/((?<= )|^)\p{Pd}+(?![\p{N}\p{Sc}])/u',
        // Remove consecutive spaces
            '/ +/',
        ),
        ' ',
        $text );
  }

	/**
   * @name _strip_views_keys($str)
   * Strips out repetetive views keys from views array keys, helpful when trying to cut down the lengths of very long views array keys
   * @internal - marked for deletion, but this is sometimes nice to have.
   */
	public static function _strip_views_keys($str) {
		$keywords = explode(";", variable_get('ph_tools_views_strip_keys_keys', ''));
		$mixedSearch 	= array();
		$mixedReplace = array();
		foreach ($keywords as $key => $value) {
			$mixedSearch[] 	= $value;
			$mixedReplace[] = '';
		}
		$str = str_replace($mixedSearch, $mixedReplace, $str);
		return $str;
	}
	
  /**
   * @name _set_header_nocache()
   * Set browser not to cache AJAX return results.
   */
	public static function _set_header_nocache() {
		header("Expires: Sun, 19 Nov 1978 05:00:00 GMT");
		header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
		header("Cache-Control: no-store, no-cache, must-revalidate");
		header("Cache-Control: post-check=0, pre-check=0", false);
		header("Pragma: no-cache");
	}
	
	/**
   * @name _browser_check()
   * @return an array $browser with the keys: $browser['version'] && $browser['type']
   */
	public static function _browser_check() {
		$useragent = $_SERVER['HTTP_USER_AGENT'];
		if (preg_match('|MSIE ([0-9].[0-9]{1,2})|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'IE';
		} 
		elseif (preg_match('|Opera ([0-9].[0-9]{1,2})|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Opera';
		} 
		elseif(preg_match('|Firefox/([0-9\.]+)|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Firefox';
		} 
		elseif(preg_match('|Safari/([0-9\.]+)|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Safari';
		} 
		else {
		// browser not recognized!
			$browser['version']  = 0;
			$browser['type']= 'other';
		}
		return $browser;
	}
	
	/**
   * @name _list_months()
   * @return array of all months listed in the Julian Calendar
   */
	public static function _list_months() {
		return array(
			'01' 	=> 'January (01)',
			'02' 	=> 'February (02)',
			'03' 	=> 'March (03)',
			'04' 	=> 'April (04)',
			'05' 	=> 'May (05)',
			'06' 	=> 'June (06)',
			'07' 	=> 'July (07)',
			'08' 	=> 'August (08)',
			'09' 	=> 'September (09)',
			'10' 	=> 'October (10)',
			'11' 	=> 'November (11)',
			'12' 	=> 'December (12)',
		);
	}
		
	/**
   * @name _list_years($startDate, $endDate)
	 * List all Years in the form of yyyy in between the start date and end date you supply.
	 * Start Date must be a date earlier than the end date
	 * @param $startDate
	 * @param $endDate
	 * @return array $years[YYYY] = YYYY
	 */
	public static function _list_years($startDate, $endDate) {
		$years = array();
		$count = $endDate - $startDate;
		$i = 0;
		if ($startDate < $endDate) {
			while ($i <= $count) {
				$newYear = $startDate + $i;
				$years[$newYear] = $newYear;
				$i++;
			}
			return $years;
		} else {
			return;
		}
	}
	
	/**
	 * @name _list_canadian_provinces()
	 * @return array a list of Canadian Provinces that CCK can understand
	 */
	public static function _list_canadian_provinces() {
		return array(
			'AB'=>'Alberta',
			'BC'=>'British Columbia',
			'LB'=>'Labrador',
			'MB'=>'Manitoba',
			'NB'=>'New Brunswick',
			'NL'=>'Newfoundland',
			'NT'=>'Northwest Territories',
			'NS'=>'Nova Scotia',
			'NU'=>'Nunavut',
			'ON'=>'Ontario',
			'PE'=>'Prince Edward Island',
			'QC'=>'Quebec',
			'SK'=>'Saskatchewan',
			'YT'=>'Yukon'
		);
	}
	
	/**
	 * @name _list_us_states()
	 * @return array a list of US states that CCK fields can easily understand
	 */
	public static function _list_us_states() {
		return array(
			'AL'=>'Alabama',
			'AK'=>'Alaska',
			'AZ'=>'Arizona',
			'AR'=>'Arkansas',
			'CA'=>'California',
			'CO'=>'Colorado',
			'CT'=>'Connecticut',
			'DE'=>'Delaware',
			'DC'=>'District Of Columbia',
			'FL'=>'Florida',
			'GA'=>'Georgia',
			'HI'=>'Hawaii',
			'ID'=>'Idaho',
			'IL'=>'Illinois',
			'IN'=>'Indiana',
			'IA'=>'Iowa',
			'KS'=>'Kansas',
			'KY'=>'Kentucky',
			'LA'=>'Louisiana',
			'ME'=>'Maine',
			'MD'=>'Maryland',
			'MA'=>'Massachusetts',
			'MI'=>'Michigan',
			'MN'=>'Minnesota',
			'MS'=>'Mississippi',
			'MO'=>'Missouri',
			'MT'=>'Montana',
			'NE'=>'Nebraska',
			'NV'=>'Nevada',
			'NH'=>'New Hampshire',
			'NJ'=>'New Jersey',
			'NM'=>'New Mexico',
			'NY'=>'New York',
			'NC'=>'North Carolina',
			'ND'=>'North Dakota',
			'OH'=>'Ohio',
			'OK'=>'Oklahoma',
			'OR'=>'Oregon',
			'PA'=>'Pennsylvania',
			'RI'=>'Rhode Island',
			'SC'=>'South Carolina',
			'SD'=>'South Dakota',
			'TN'=>'Tennessee',
			'TX'=>'Texas',
			'UT'=>'Utah',
			'VT'=>'Vermont',
			'VA'=>'Virginia',
			'WA'=>'Washington',
			'WV'=>'West Virginia',
			'WI'=>'Wisconsin',
			'WY'=>'Wyoming',
   );
	}
	
  /**
   * @name _list_countries_iso()
   * A list of current world countries and their ISO code, useful when using shipping
   * @return array of countries, with the keys: upper lower alpha3 numeric
   */
	public static function _list_countries_iso() {
		return array(
			'AF' => array('upper'=>'AFGHANISTAN','lower'=>'Afghanistan','alpha3'=>'AFG','numeric'=>'004'),
			'AL' => array('upper'=>'ALBANIA','lower'=>'Albania','alpha3'=>'ALB','numeric'=>'008'),
			'DZ' => array('upper'=>'ALGERIA','lower'=>'Algeria','alpha3'=>'DZA','numeric'=>'012'),
			'AS' => array('upper'=>'AMERICAN SAMOA','lower'=>'American Samoa','alpha3'=>'ASM','numeric'=>'016'),
			'AD' => array('upper'=>'ANDORRA','lower'=>'Andorra','alpha3'=>'AND','numeric'=>'020'),
			'AO' => array('upper'=>'ANGOLA','lower'=>'Angola','alpha3'=>'AGO','numeric'=>'024'),
			'AI' => array('upper'=>'ANGUILLA','lower'=>'Anguilla','alpha3'=>'AIA','numeric'=>'660'),
			'AQ' => array('upper'=>'ANTARCTICA','lower'=>'Antarctica','alpha3'=>NULL,'numeric'=>NULL),
			'AG' => array('upper'=>'ANTIGUA AND BARBUDA','lower'=>'Antigua and Barbuda','alpha3'=>'ATG','numeric'=>'028'),
			'AR' => array('upper'=>'ARGENTINA','lower'=>'Argentina','alpha3'=>'ARG','numeric'=>'032'),
			'AM' => array('upper'=>'ARMENIA','lower'=>'Armenia','alpha3'=>'ARM','numeric'=>'051'),
			'AW' => array('upper'=>'ARUBA','lower'=>'Aruba','alpha3'=>'ABW','numeric'=>'533'),
			'AU' => array('upper'=>'AUSTRALIA','lower'=>'Australia','alpha3'=>'AUS','numeric'=>'036'),
			'AT' => array('upper'=>'AUSTRIA','lower'=>'Austria','alpha3'=>'AUT','numeric'=>'040'),
			'AZ' => array('upper'=>'AZERBAIJAN','lower'=>'Azerbaijan','alpha3'=>'AZE','numeric'=>'031'),
			'BS' => array('upper'=>'BAHAMAS','lower'=>'Bahamas','alpha3'=>'BHS','numeric'=>'044'),
			'BH' => array('upper'=>'BAHRAIN','lower'=>'Bahrain','alpha3'=>'BHR','numeric'=>'048'),
			'BD' => array('upper'=>'BANGLADESH','lower'=>'Bangladesh','alpha3'=>'BGD','numeric'=>'050'),
			'BB' => array('upper'=>'BARBADOS','lower'=>'Barbados','alpha3'=>'BRB','numeric'=>'052'),
			'BY' => array('upper'=>'BELARUS','lower'=>'Belarus','alpha3'=>'BLR','numeric'=>'112'),
			'BE' => array('upper'=>'BELGIUM','lower'=>'Belgium','alpha3'=>'BEL','numeric'=>'056'),
			'BZ' => array('upper'=>'BELIZE','lower'=>'Belize','alpha3'=>'BLZ','numeric'=>'084'),
			'BJ' => array('upper'=>'BENIN','lower'=>'Benin','alpha3'=>'BEN','numeric'=>'204'),
			'BM' => array('upper'=>'BERMUDA','lower'=>'Bermuda','alpha3'=>'BMU','numeric'=>'060'),
			'BT' => array('upper'=>'BHUTAN','lower'=>'Bhutan','alpha3'=>'BTN','numeric'=>'064'),
			'BO' => array('upper'=>'BOLIVIA','lower'=>'Bolivia','alpha3'=>'BOL','numeric'=>'068'),
			'BA' => array('upper'=>'BOSNIA AND HERZEGOVINA','lower'=>'Bosnia and Herzegovina','alpha3'=>'BIH','numeric'=>'070'),
			'BW' => array('upper'=>'BOTSWANA','lower'=>'Botswana','alpha3'=>'BWA','numeric'=>'072'),
			'BV' => array('upper'=>'BOUVET ISLAND','lower'=>'Bouvet Island','alpha3'=>NULL,'numeric'=>NULL),
			'BR' => array('upper'=>'BRAZIL','lower'=>'Brazil','alpha3'=>'BRA','numeric'=>'076'),
			'IO' => array('upper'=>'BRITISH INDIAN OCEAN TERRITORY','lower'=>'British Indian Ocean Territory','alpha3'=>NULL,'numeric'=>NULL),
			'BN' => array('upper'=>'BRUNEI DARUSSALAM','lower'=>'Brunei Darussalam','alpha3'=>'BRN','numeric'=>'096'),
			'BG' => array('upper'=>'BULGARIA','lower'=>'Bulgaria','alpha3'=>'BGR','numeric'=>'100'),
			'BF' => array('upper'=>'BURKINA FASO','lower'=>'Burkina Faso','alpha3'=>'BFA','numeric'=>'854'),
			'BI' => array('upper'=>'BURUNDI','lower'=>'Burundi','alpha3'=>'BDI','numeric'=>'108'),
			'KH' => array('upper'=>'CAMBODIA','lower'=>'Cambodia','alpha3'=>'KHM','numeric'=>'116'),
			'CM' => array('upper'=>'CAMEROON','lower'=>'Cameroon','alpha3'=>'CMR','numeric'=>'120'),
			'CA' => array('upper'=>'CANADA','lower'=>'Canada','alpha3'=>'CAN','numeric'=>'124'),
			'CV' => array('upper'=>'CAPE VERDE','lower'=>'Cape Verde','alpha3'=>'CPV','numeric'=>'132'),
			'KY' => array('upper'=>'CAYMAN ISLANDS','lower'=>'Cayman Islands','alpha3'=>'CYM','numeric'=>'136'),
			'CF' => array('upper'=>'CENTRAL AFRICAN REPUBLIC','lower'=>'Central African Republic','alpha3'=>'CAF','numeric'=>'140'),
			'TD' => array('upper'=>'CHAD','lower'=>'Chad','alpha3'=>'TCD','numeric'=>'148'),
			'CL' => array('upper'=>'CHILE','lower'=>'Chile','alpha3'=>'CHL','numeric'=>'152'),
			'CN' => array('upper'=>'CHINA','lower'=>'China','alpha3'=>'CHN','numeric'=>'156'),
			'CX' => array('upper'=>'CHRISTMAS ISLAND','lower'=>'Christmas Island','alpha3'=>NULL,'numeric'=>NULL),
			'CC' => array('upper'=>'COCOSKEELING ISLANDS','lower'=>'CocosKeeling Islands','alpha3'=>NULL,'numeric'=>NULL),
			'CO' => array('upper'=>'COLOMBIA','lower'=>'Colombia','alpha3'=>'COL','numeric'=>'170'),
			'KM' => array('upper'=>'COMOROS','lower'=>'Comoros','alpha3'=>'COM','numeric'=>'174'),
			'CG' => array('upper'=>'CONGO','lower'=>'Congo','alpha3'=>'COG','numeric'=>'178'),
			'CD' => array('upper'=>'CONGO, THE DEMOCRATIC REPUBLIC OF THE','lower'=>'Congo, the Democratic Republic of the','alpha3'=>'COD','numeric'=>'180'),
			'CK' => array('upper'=>'COOK ISLANDS','lower'=>'Cook Islands','alpha3'=>'COK','numeric'=>'184'),
			'CR' => array('upper'=>'COSTA RICA','lower'=>'Costa Rica','alpha3'=>'CRI','numeric'=>'188'),
			'CI' => array('upper'=>'COTE D\'IVOIRE','lower'=>'Cote D\'Ivoire','alpha3'=>'CIV','numeric'=>'384'),
			'HR' => array('upper'=>'CROATIA','lower'=>'Croatia','alpha3'=>'HRV','numeric'=>'191'),
			'CU' => array('upper'=>'CUBA','lower'=>'Cuba','alpha3'=>'CUB','numeric'=>'192'),
			'CY' => array('upper'=>'CYPRUS','lower'=>'Cyprus','alpha3'=>'CYP','numeric'=>'196'),
			'CZ' => array('upper'=>'CZECH REPUBLIC','lower'=>'Czech Republic','alpha3'=>'CZE','numeric'=>'203'),
			'DK' => array('upper'=>'DENMARK','lower'=>'Denmark','alpha3'=>'DNK','numeric'=>'208'),
			'DJ' => array('upper'=>'DJIBOUTI','lower'=>'Djibouti','alpha3'=>'DJI','numeric'=>'262'),
			'DM' => array('upper'=>'DOMINICA','lower'=>'Dominica','alpha3'=>'DMA','numeric'=>'212'),
			'DO' => array('upper'=>'DOMINICAN REPUBLIC','lower'=>'Dominican Republic','alpha3'=>'DOM','numeric'=>'214'),
			'EC' => array('upper'=>'ECUADOR','lower'=>'Ecuador','alpha3'=>'ECU','numeric'=>'218'),
			'EG' => array('upper'=>'EGYPT','lower'=>'Egypt','alpha3'=>'EGY','numeric'=>'818'),
			'SV' => array('upper'=>'EL SALVADOR','lower'=>'El Salvador','alpha3'=>'SLV','numeric'=>'222'),
			'GQ' => array('upper'=>'EQUATORIAL GUINEA','lower'=>'Equatorial Guinea','alpha3'=>'GNQ','numeric'=>'226'),
			'ER' => array('upper'=>'ERITREA','lower'=>'Eritrea','alpha3'=>'ERI','numeric'=>'232'),
			'EE' => array('upper'=>'ESTONIA','lower'=>'Estonia','alpha3'=>'EST','numeric'=>'233'),
			'ET' => array('upper'=>'ETHIOPIA','lower'=>'Ethiopia','alpha3'=>'ETH','numeric'=>'231'),
			'FK' => array('upper'=>'FALKLAND ISLANDSMALVINAS','lower'=>'Falkland IslandsMalvinas','alpha3'=>'FLK','numeric'=>'238'),
			'FO' => array('upper'=>'FAROE ISLANDS','lower'=>'Faroe Islands','alpha3'=>'FRO','numeric'=>'234'),
			'FJ' => array('upper'=>'FIJI','lower'=>'Fiji','alpha3'=>'FJI','numeric'=>'242'),
			'FI' => array('upper'=>'FINLAND','lower'=>'Finland','alpha3'=>'FIN','numeric'=>'246'),
			'FR' => array('upper'=>'FRANCE','lower'=>'France','alpha3'=>'FRA','numeric'=>'250'),
			'GF' => array('upper'=>'FRENCH GUIANA','lower'=>'French Guiana','alpha3'=>'GUF','numeric'=>'254'),
			'PF' => array('upper'=>'FRENCH POLYNESIA','lower'=>'French Polynesia','alpha3'=>'PYF','numeric'=>'258'),
			'TF' => array('upper'=>'FRENCH SOUTHERN TERRITORIES','lower'=>'French Southern Territories','alpha3'=>NULL,'numeric'=>NULL),
			'GA' => array('upper'=>'GABON','lower'=>'Gabon','alpha3'=>'GAB','numeric'=>'266'),
			'GM' => array('upper'=>'GAMBIA','lower'=>'Gambia','alpha3'=>'GMB','numeric'=>'270'),
			'GE' => array('upper'=>'GEORGIA','lower'=>'Georgia','alpha3'=>'GEO','numeric'=>'268'),
			'DE' => array('upper'=>'GERMANY','lower'=>'Germany','alpha3'=>'DEU','numeric'=>'276'),
			'GH' => array('upper'=>'GHANA','lower'=>'Ghana','alpha3'=>'GHA','numeric'=>'288'),
			'GI' => array('upper'=>'GIBRALTAR','lower'=>'Gibraltar','alpha3'=>'GIB','numeric'=>'292'),
			'GR' => array('upper'=>'GREECE','lower'=>'Greece','alpha3'=>'GRC','numeric'=>'300'),
			'GL' => array('upper'=>'GREENLAND','lower'=>'Greenland','alpha3'=>'GRL','numeric'=>'304'),
			'GD' => array('upper'=>'GRENADA','lower'=>'Grenada','alpha3'=>'GRD','numeric'=>'308'),
			'GP' => array('upper'=>'GUADELOUPE','lower'=>'Guadeloupe','alpha3'=>'GLP','numeric'=>'312'),
			'GU' => array('upper'=>'GUAM','lower'=>'Guam','alpha3'=>'GUM','numeric'=>'316'),
			'GT' => array('upper'=>'GUATEMALA','lower'=>'Guatemala','alpha3'=>'GTM','numeric'=>'320'),
			'GN' => array('upper'=>'GUINEA','lower'=>'Guinea','alpha3'=>'GIN','numeric'=>'324'),
			'GW' => array('upper'=>'GUINEA-BISSAU','lower'=>'Guinea-Bissau','alpha3'=>'GNB','numeric'=>'624'),
			'GY' => array('upper'=>'GUYANA','lower'=>'Guyana','alpha3'=>'GUY','numeric'=>'328'),
			'HT' => array('upper'=>'HAITI','lower'=>'Haiti','alpha3'=>'HTI','numeric'=>'332'),
			'HM' => array('upper'=>'HEARD ISLAND AND MCDONALD ISLANDS','lower'=>'Heard Island and Mcdonald Islands','alpha3'=>NULL,'numeric'=>NULL),
			'VA' => array('upper'=>'HOLY SEE (VATICAN CITY STATE)','lower'=>'Holy See (Vatican City State)','alpha3'=>'VAT','numeric'=>'336'),
			'HN' => array('upper'=>'HONDURAS','lower'=>'Honduras','alpha3'=>'HND','numeric'=>'340'),
			'HK' => array('upper'=>'HONG KONG','lower'=>'Hong Kong','alpha3'=>'HKG','numeric'=>'344'),
			'HU' => array('upper'=>'HUNGARY','lower'=>'Hungary','alpha3'=>'HUN','numeric'=>'348'),
			'IS' => array('upper'=>'ICELAND','lower'=>'Iceland','alpha3'=>'ISL','numeric'=>'352'),
			'IN' => array('upper'=>'INDIA','lower'=>'India','alpha3'=>'IND','numeric'=>'356'),
			'ID' => array('upper'=>'INDONESIA','lower'=>'Indonesia','alpha3'=>'IDN','numeric'=>'360'),
			'IR' => array('upper'=>'IRAN, ISLAMIC REPUBLIC OF','lower'=>'Iran, Islamic Republic of','alpha3'=>'IRN','numeric'=>'364'),
			'IQ' => array('upper'=>'IRAQ','lower'=>'Iraq','alpha3'=>'IRQ','numeric'=>'368'),
			'IE' => array('upper'=>'IRELAND','lower'=>'Ireland','alpha3'=>'IRL','numeric'=>'372'),
			'IL' => array('upper'=>'ISRAEL','lower'=>'Israel','alpha3'=>'ISR','numeric'=>'376'),
			'IT' => array('upper'=>'ITALY','lower'=>'Italy','alpha3'=>'ITA','numeric'=>'380'),
			'JM' => array('upper'=>'JAMAICA','lower'=>'Jamaica','alpha3'=>'JAM','numeric'=>'388'),
			'JP' => array('upper'=>'JAPAN','lower'=>'Japan','alpha3'=>'JPN','numeric'=>'392'),
			'JO' => array('upper'=>'JORDAN','lower'=>'Jordan','alpha3'=>'JOR','numeric'=>'400'),
			'KZ' => array('upper'=>'KAZAKHSTAN','lower'=>'Kazakhstan','alpha3'=>'KAZ','numeric'=>'398'),
			'KE' => array('upper'=>'KENYA','lower'=>'Kenya','alpha3'=>'KEN','numeric'=>'404'),
			'KI' => array('upper'=>'KIRIBATI','lower'=>'Kiribati','alpha3'=>'KIR','numeric'=>'296'),
			'KP' => array('upper'=>'KOREA', 'lower'=>'Korea, Democratic People\'s Republic of','alpha3'=>'PRK','numeric'=>'408'),
			'KR' => array('upper'=>'KOREA', 'lower'=>'Korea, Republic of','alpha3'=>'KOR','numeric'=>'410'),
			'KW' => array('upper'=>'KUWAIT','lower'=>'Kuwait','alpha3'=>'KWT','numeric'=>'414'),
			'KG' => array('upper'=>'KYRGYZSTAN','lower'=>'Kyrgyzstan','alpha3'=>'KGZ','numeric'=>'417'),
			'LA' => array('upper'=>'LAO PEOPLE\'S DEMOCRATIC REPUBLIC','lower'=>'Lao People\'s Democratic Republic','alpha3'=>'LAO','numeric'=>'418'),
			'LV' => array('upper'=>'LATVIA','lower'=>'Latvia','alpha3'=>'LVA','numeric'=>'428'),
			'LB' => array('upper'=>'LEBANON','lower'=>'Lebanon','alpha3'=>'LBN','numeric'=>'422'),
			'LS' => array('upper'=>'LESOTHO','lower'=>'Lesotho','alpha3'=>'LSO','numeric'=>'426'),
			'LR' => array('upper'=>'LIBERIA','lower'=>'Liberia','alpha3'=>'LBR','numeric'=>'430'),
			'LY' => array('upper'=>'LIBYAN ARAB JAMAHIRIYA','lower'=>'Libyan Arab Jamahiriya','alpha3'=>'LBY','numeric'=>'434'),
			'LI' => array('upper'=>'LIECHTENSTEIN','lower'=>'Liechtenstein','alpha3'=>'LIE','numeric'=>'438'),
			'LT' => array('upper'=>'LITHUANIA','lower'=>'Lithuania','alpha3'=>'LTU','numeric'=>'440'),
			'LU' => array('upper'=>'LUXEMBOURG','lower'=>'Luxembourg','alpha3'=>'LUX','numeric'=>'442'),
			'MO' => array('upper'=>'MACAO','lower'=>'Macao','alpha3'=>'MAC','numeric'=>'446'),
			'MK' => array('upper'=>'MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF','lower'=>'Macedonia, the Former Yugoslav Republic of','alpha3'=>'MKD','numeric'=>'807'),
			'MG' => array('upper'=>'MADAGASCAR','lower'=>'Madagascar','alpha3'=>'MDG','numeric'=>'450'),
			'MW' => array('upper'=>'MALAWI','lower'=>'Malawi','alpha3'=>'MWI','numeric'=>'454'),
			'MY' => array('upper'=>'MALAYSIA','lower'=>'Malaysia','alpha3'=>'MYS','numeric'=>'458'),
			'MV' => array('upper'=>'MALDIVES','lower'=>'Maldives','alpha3'=>'MDV','numeric'=>'462'),
			'ML' => array('upper'=>'MALI','lower'=>'Mali','alpha3'=>'MLI','numeric'=>'466'),
			'MT' => array('upper'=>'MALTA','lower'=>'Malta','alpha3'=>'MLT','numeric'=>'470'),
			'MH' => array('upper'=>'MARSHALL ISLANDS','lower'=>'Marshall Islands','alpha3'=>'MHL','numeric'=>'584'),
			'MQ' => array('upper'=>'MARTINIQUE','lower'=>'Martinique','alpha3'=>'MTQ','numeric'=>'474'),
			'MR' => array('upper'=>'MAURITANIA','lower'=>'Mauritania','alpha3'=>'MRT','numeric'=>'478'),
			'MU' => array('upper'=>'MAURITIUS','lower'=>'Mauritius','alpha3'=>'MUS','numeric'=>'480'),
			'YT' => array('upper'=>'MAYOTTE','lower'=>'Mayotte','alpha3'=>NULL,'numeric'=>NULL),
			'MX' => array('upper'=>'MEXICO','lower'=>'Mexico','alpha3'=>'MEX','numeric'=>'484'),
			'FM' => array('upper'=>'MICRONESIA, FEDERATED STATES OF','lower'=>'Micronesia, Federated States of','alpha3'=>'FSM','numeric'=>'583'),
			'MD' => array('upper'=>'MOLDOVA, REPUBLIC OF','lower'=>'Moldova, Republic of','alpha3'=>'MDA','numeric'=>'498'),
			'MC' => array('upper'=>'MONACO','lower'=>'Monaco','alpha3'=>'MCO','numeric'=>'492'),
			'MN' => array('upper'=>'MONGOLIA','lower'=>'Mongolia','alpha3'=>'MNG','numeric'=>'496'),
			'MS' => array('upper'=>'MONTSERRAT','lower'=>'Montserrat','alpha3'=>'MSR','numeric'=>'500'),
			'MA' => array('upper'=>'MOROCCO','lower'=>'Morocco','alpha3'=>'MAR','numeric'=>'504'),
			'MZ' => array('upper'=>'MOZAMBIQUE','lower'=>'Mozambique','alpha3'=>'MOZ','numeric'=>'508'),
			'MM' => array('upper'=>'MYANMAR','lower'=>'Myanmar','alpha3'=>'MMR','numeric'=>'104'),
			'NA' => array('upper'=>'NAMIBIA','lower'=>'Namibia','alpha3'=>'NAM','numeric'=>'516'),
			'NR' => array('upper'=>'NAURU','lower'=>'Nauru','alpha3'=>'NRU','numeric'=>'520'),
			'NP' => array('upper'=>'NEPAL','lower'=>'Nepal','alpha3'=>'NPL','numeric'=>'524'),
			'NL' => array('upper'=>'NETHERLANDS','lower'=>'Netherlands','alpha3'=>'NLD','numeric'=>'528'),
			'AN' => array('upper'=>'NETHERLANDS ANTILLES','lower'=>'Netherlands Antilles','alpha3'=>'ANT','numeric'=>'530'),
			'NC' => array('upper'=>'NEW CALEDONIA','lower'=>'New Caledonia','alpha3'=>'NCL','numeric'=>'540'),
			'NZ' => array('upper'=>'NEW ZEALAND','lower'=>'New Zealand','alpha3'=>'NZL','numeric'=>'554'),
			'NI' => array('upper'=>'NICARAGUA','lower'=>'Nicaragua','alpha3'=>'NIC','numeric'=>'558'),
			'NE' => array('upper'=>'NIGER','lower'=>'Niger','alpha3'=>'NER','numeric'=>'562'),
			'NG' => array('upper'=>'NIGERIA','lower'=>'Nigeria','alpha3'=>'NGA','numeric'=>'566'),
			'NU' => array('upper'=>'NIUE','lower'=>'Niue','alpha3'=>'NIU','numeric'=>'570'),
			'NF' => array('upper'=>'NORFOLK ISLAND','lower'=>'Norfolk Island','alpha3'=>'NFK','numeric'=>'574'),
			'MP' => array('upper'=>'NORTHERN MARIANA ISLANDS','lower'=>'Northern Mariana Islands','alpha3'=>'MNP','numeric'=>'580'),
			'NO' => array('upper'=>'NORWAY','lower'=>'Norway','alpha3'=>'NOR','numeric'=>'578'),
			'OM' => array('upper'=>'OMAN','lower'=>'Oman','alpha3'=>'OMN','numeric'=>'512'),
			'PK' => array('upper'=>'PAKISTAN','lower'=>'Pakistan','alpha3'=>'PAK','numeric'=>'586'),
			'PW' => array('upper'=>'PALAU','lower'=>'Palau','alpha3'=>'PLW','numeric'=>'585'),
			'PS' => array('upper'=>'PALESTINIAN TERRITORY, OCCUPIED','lower'=>'Palestinian Territory, Occupied','alpha3'=>NULL,'numeric'=>NULL),
			'PA' => array('upper'=>'PANAMA','lower'=>'Panama','alpha3'=>'PAN','numeric'=>'591'),
			'PG' => array('upper'=>'PAPUA NEW GUINEA','lower'=>'Papua New Guinea','alpha3'=>'PNG','numeric'=>'598'),
			'PY' => array('upper'=>'PARAGUAY','lower'=>'Paraguay','alpha3'=>'PRY','numeric'=>'600'),
			'PE' => array('upper'=>'PERU','lower'=>'Peru','alpha3'=>'PER','numeric'=>'604'),
			'PH' => array('upper'=>'PHILIPPINES','lower'=>'Philippines','alpha3'=>'PHL','numeric'=>'608'),
			'PN' => array('upper'=>'PITCAIRN','lower'=>'Pitcairn','alpha3'=>'PCN','numeric'=>'612'),
			'PL' => array('upper'=>'POLAND','lower'=>'Poland','alpha3'=>'POL','numeric'=>'616'),
			'PT' => array('upper'=>'PORTUGAL','lower'=>'Portugal','alpha3'=>'PRT','numeric'=>'620'),
			'PR' => array('upper'=>'PUERTO RICO','lower'=>'Puerto Rico','alpha3'=>'PRI','numeric'=>'630'),
			'QA' => array('upper'=>'QATAR','lower'=>'Qatar','alpha3'=>'QAT','numeric'=>'634'),
			'RE' => array('upper'=>'REUNION','lower'=>'Reunion','alpha3'=>'REU','numeric'=>'638'),
			'RO' => array('upper'=>'ROMANIA','lower'=>'Romania','alpha3'=>'ROM','numeric'=>'642'),
			'RU' => array('upper'=>'RUSSIAN FEDERATION','lower'=>'Russian Federation','alpha3'=>'RUS','numeric'=>'643'),
			'RW' => array('upper'=>'RWANDA','lower'=>'Rwanda','alpha3'=>'RWA','numeric'=>'646'),
			'SH' => array('upper'=>'SAINT HELENA','lower'=>'Saint Helena','alpha3'=>'SHN','numeric'=>'654'),
			'KN' => array('upper'=>'SAINT KITTS AND NEVIS','lower'=>'Saint Kitts and Nevis','alpha3'=>'KNA','numeric'=>'659'),
			'LC' => array('upper'=>'SAINT LUCIA','lower'=>'Saint Lucia','alpha3'=>'LCA','numeric'=>'662'),
			'PM' => array('upper'=>'SAINT PIERRE AND MIQUELON','lower'=>'Saint Pierre and Miquelon','alpha3'=>'SPM','numeric'=>'666'),
			'VC' => array('upper'=>'SAINT VINCENT AND THE GRENADINES','lower'=>'Saint Vincent and the Grenadines','alpha3'=>'VCT','numeric'=>'670'),
			'WS' => array('upper'=>'SAMOA','lower'=>'Samoa','alpha3'=>'WSM','numeric'=>'882'),
			'SM' => array('upper'=>'SAN MARINO','lower'=>'San Marino','alpha3'=>'SMR','numeric'=>'674'),
			'ST' => array('upper'=>'SAO TOME AND PRINCIPE','lower'=>'Sao Tome and Principe','alpha3'=>'STP','numeric'=>'678'),
			'SA' => array('upper'=>'SAUDI ARABIA','lower'=>'Saudi Arabia','alpha3'=>'SAU','numeric'=>'682'),
			'SN' => array('upper'=>'SENEGAL','lower'=>'Senegal','alpha3'=>'SEN','numeric'=>'686'),
			'CS' => array('upper'=>'SERBIA AND MONTENEGRO','lower'=>'Serbia and Montenegro','alpha3'=>NULL,'numeric'=>NULL),
			'SC' => array('upper'=>'SEYCHELLES','lower'=>'Seychelles','alpha3'=>'SYC','numeric'=>'690'),
			'SL' => array('upper'=>'SIERRA LEONE','lower'=>'Sierra Leone','alpha3'=>'SLE','numeric'=>'694'),
			'SG' => array('upper'=>'SINGAPORE','lower'=>'Singapore','alpha3'=>'SGP','numeric'=>'702'),
			'SK' => array('upper'=>'SLOVAKIA','lower'=>'Slovakia','alpha3'=>'SVK','numeric'=>'703'),
			'SI' => array('upper'=>'SLOVENIA','lower'=>'Slovenia','alpha3'=>'SVN','numeric'=>'705'),
			'SB' => array('upper'=>'SOLOMON ISLANDS','lower'=>'Solomon Islands','alpha3'=>'SLB','numeric'=>'090'),
			'SO' => array('upper'=>'SOMALIA','lower'=>'Somalia','alpha3'=>'SOM','numeric'=>'706'),
			'ZA' => array('upper'=>'SOUTH AFRICA','lower'=>'South Africa','alpha3'=>'ZAF','numeric'=>'710'),
			'GS' => array('upper'=>'SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS','lower'=>'South Georgia and the South Sandwich Islands','alpha3'=>NULL,'numeric'=>NULL),
			'ES' => array('upper'=>'SPAIN','lower'=>'Spain','alpha3'=>'ESP','numeric'=>'724'),
			'LK' => array('upper'=>'SRI LANKA','lower'=>'Sri Lanka','alpha3'=>'LKA','numeric'=>'144'),
			'SD' => array('upper'=>'SUDAN','lower'=>'Sudan','alpha3'=>'SDN','numeric'=>'736'),
			'SR' => array('upper'=>'SURINAME','lower'=>'Suriname','alpha3'=>'SUR','numeric'=>'740'),
			'SJ' => array('upper'=>'SVALBARD AND JAN MAYEN','lower'=>'Svalbard and Jan Mayen','alpha3'=>'SJM','numeric'=>'744'),
			'SZ' => array('upper'=>'SWAZILAND','lower'=>'Swaziland','alpha3'=>'SWZ','numeric'=>'748'),
			'SE' => array('upper'=>'SWEDEN','lower'=>'Sweden','alpha3'=>'SWE','numeric'=>'752'),
			'CH' => array('upper'=>'SWITZERLAND','lower'=>'Switzerland','alpha3'=>'CHE','numeric'=>'756'),
			'SY' => array('upper'=>'SYRIAN ARAB REPUBLIC','lower'=>'Syrian Arab Republic','alpha3'=>'SYR','numeric'=>'760'),
			'TW' => array('upper'=>'TAIWAN, PROVINCE OF CHINA','lower'=>'Taiwan, Province of China','alpha3'=>'TWN','numeric'=>'158'),
			'TJ' => array('upper'=>'TAJIKISTAN','lower'=>'Tajikistan','alpha3'=>'TJK','numeric'=>'762'),
			'TZ' => array('upper'=>'TANZANIA, UNITED REPUBLIC OF','lower'=>'Tanzania, United Republic of','alpha3'=>'TZA','numeric'=>'834'),
			'TH' => array('upper'=>'THAILAND','lower'=>'Thailand','alpha3'=>'THA','numeric'=>'764'),
			'TL' => array('upper'=>'TIMOR-LESTE','lower'=>'Timor-Leste','alpha3'=>NULL,'numeric'=>NULL),
			'TG' => array('upper'=>'TOGO','lower'=>'Togo','alpha3'=>'TGO','numeric'=>'768'),
			'TK' => array('upper'=>'TOKELAU','lower'=>'Tokelau','alpha3'=>'TKL','numeric'=>'772'),
			'TO' => array('upper'=>'TONGA','lower'=>'Tonga','alpha3'=>'TON','numeric'=>'776'),
			'TT' => array('upper'=>'TRINIDAD AND TOBAGO','lower'=>'Trinidad and Tobago','alpha3'=>'TTO','numeric'=>'780'),
			'TN' => array('upper'=>'TUNISIA','lower'=>'Tunisia','alpha3'=>'TUN','numeric'=>'788'),
			'TR' => array('upper'=>'TURKEY','lower'=>'Turkey','alpha3'=>'TUR','numeric'=>'792'),
			'TM' => array('upper'=>'TURKMENISTAN','lower'=>'Turkmenistan','alpha3'=>'TKM','numeric'=>'795'),
			'TC' => array('upper'=>'TURKS AND CAICOS ISLANDS','lower'=>'Turks and Caicos Islands','alpha3'=>'TCA','numeric'=>'796'),
			'TV' => array('upper'=>'TUVALU','lower'=>'Tuvalu','alpha3'=>'TUV','numeric'=>'798'),
			'UG' => array('upper'=>'UGANDA','lower'=>'Uganda','alpha3'=>'UGA','numeric'=>'800'),
			'UA' => array('upper'=>'UKRAINE','lower'=>'Ukraine','alpha3'=>'UKR','numeric'=>'804'),
			'AE' => array('upper'=>'UNITED ARAB EMIRATES','lower'=>'United Arab Emirates','alpha3'=>'ARE','numeric'=>'784'),
			'GB' => array('upper'=>'UNITED KINGDOM','lower'=>'United Kingdom','alpha3'=>'GBR','numeric'=>'826'),
			'US' => array('upper'=>'UNITED STATES','lower'=>'United States','alpha3'=>'USA','numeric'=>'840'),
			'UM' => array('upper'=>'UNITED STATES MINOR OUTLYING ISLANDS','lower'=>'United States Minor Outlying Islands','alpha3'=>NULL,'numeric'=>NULL),
			'UY' => array('upper'=>'URUGUAY','lower'=>'Uruguay','alpha3'=>'URY','numeric'=>'858'),
			'UZ' => array('upper'=>'UZBEKISTAN','lower'=>'Uzbekistan','alpha3'=>'UZB','numeric'=>'860'),
			'VU' => array('upper'=>'VANUATU','lower'=>'Vanuatu','alpha3'=>'VUT','numeric'=>'548'),
			'VE' => array('upper'=>'VENEZUELA','lower'=>'Venezuela','alpha3'=>'VEN','numeric'=>'862'),
			'VN' => array('upper'=>'VIET NAM','lower'=>'Viet Nam','alpha3'=>'VNM','numeric'=>'704'),
			'VG' => array('upper'=>'VIRGIN ISLANDS, BRITISH','lower'=>'Virgin Islands, British','alpha3'=>'VGB','numeric'=>'092'),
			'VI' => array('upper'=>'VIRGIN ISLANDS, U.S.','lower'=>'Virgin Islands, U.s.','alpha3'=>'VIR','numeric'=>'850'),
			'WF' => array('upper'=>'WALLIS AND FUTUNA','lower'=>'Wallis and Futuna','alpha3'=>'WLF','numeric'=>'876'),
			'EH' => array('upper'=>'WESTERN SAHARA','lower'=>'Western Sahara','alpha3'=>'ESH','numeric'=>'732'),
			'YE' => array('upper'=>'YEMEN','lower'=>'Yemen','alpha3'=>'YEM','numeric'=>'887'),
			'ZM' => array('upper'=>'ZAMBIA','lower'=>'Zambia','alpha3'=>'ZMB','numeric'=>'894'),
			'ZW' => array('upper'=>'ZIMBABWE','lower'=>'Zimbabwe','alpha3'=>'ZWE','numeric'=>'716')
		);
	}
  
  /**
   * @name _list_countries_iso_cck()
   * @return array a list of countries that CCK fields can easily understand
   */
  public static function _list_countries_iso_cck() {
    $countries  = phTools::_list_countries_iso();
    $countryArr = array('NULL' => '- None -');
    foreach ($countries as $key => $value) {
      $countryArr[$key] = $value['lower'];
    }
    return $countryArr;
  }
  
	/**
   * @name _get_season($to = 0)
   * Get the season name, Winter, Autumn, Summer, Spring, Winter and it's date
   * @param integer $to a value in the future for the season you wish to select
   * @return array $season['name'] $season['date']
   */
	public static function _get_season($to = 0) {
		if (is_numeric($to)) {
			$seasonDates = array('/12/21'=>'Winter', '/09/21'=>'Autumn', '/06/21'=>'Summer', '/03/21'=>'Spring', '/01/01'=>'Winter');
			$season = array();
			foreach ($seasonDates as $key => $value) {
				$seasonDate = date("Y", strtotime("now + $to days")) .$key;
				if (strtotime("now + $to days") > strtotime($seasonDate)) {
					$season['name'] = $value;
					$season['date'] = $key;
					return $season;
				}
			}
		} else {
			drupal_set_message('You must use a numeric value for the variable $to _getSeason($to), please look at this public static function and enter in a numeric value.', 'error');
		}
	}
  
  /**
   * Validation routines, for checking user information.
   */
  
  /**
   * _validateData($data = null)
   * @param object $data = the data you wish to validate
   * @return mixed returns either boolean false or the xml object, so you can check for success
   */
  public static function _validateData($data = null) { //Queries the XML source for a valid object
    $data = simplexml_load_string($data);
    if (is_object($data)) { //return the valid data object, this is a prime example of how you can override something through a child class
      return $data;
    } else { //or return false, so you can add your own success check
      return false;
    }
  }
  
  /**
   * _validateURL($url = null)
   * @param string $url = the url you wish to validate
   * @return mixed returns either boolean false or the xml object, so you can check for success
   */
  public static function _validateURL($url = null) {
    return (valid_url($url, true)) ? $url : false;
  }

  /**
   * _validateFileName($filename = null)
   * Mimics file_validate_extensions without returning form errors.
   * @param string $filename = the filename you wish to validate
   * @return mixed returns either boolean false or the file, so you can check for success
   */
  public static function _validateFileName($filename = null) {
    (preg_match('/\.(png|gif|jpe?g)$/i', $filename, $filematches)) ? TRUE : FALSE;
  }

}

/**
 * @name phToolsWidgets
 */

class phToolsWidgets {
  
  /**
    * @name _get_weather_widget($url)
    * returns a weather widget from NOAA's XML
    * @param string $url a valid url for creating the widget
    * @return html of the weather widget from the $widget array
    * $widget->icon_url_base
    * $widget->icon_url_name
    * $widget->weather
    * $widget->temperature_string
    * $widget->wind_string
    * $widget->relative_humidity
    * $widget->dewpoint_string
    * $widget->visibility_mi
    * $widget->windchill_string
    * $widget->observation_time
    * $widget->credit
  */
  public static function _get_weather_widget($url) {
    
    if (!empty($url) && strstr($url, 'http://www.weather.gov/xml/current_obs')) { //check to see if the url is valid
      
      $url = $url . '?nocache=' .md5(time()); //make sure the xml does not cache
      $xml = simplexml_load_file ($url);
      
      //print_r($xml);
      
      $widget = new stdClass;
      
      foreach ($xml as $key => $value) {
        
        $widget->$key = (string) trim($value);
        
      }
      
      return theme('weather_widget', $widget);
      
    } else {
      
      drupal_set_message(t('You have not entered in a valid url, please enter in a url destination from http://www.weather.gov/xml/current_obs'), 'error');
      
    }
    
  }

}

/**
 * @name phToolsAdmin
 */
class phToolsAdmin {
  
  /**
   * @name _checkLibraries
   * Check the path you have given for jquery libraries
   */
  public static function _checkLibrary() {
    $whitelist = variable_get('ph_tools_library_whitelist', PH_TOOLS_MODULE_WHITELIST);
    $whitelist = str_replace(array("\r","\n"), array("","|"), $whitelist);
    $files = file_scan_directory(variable_get('ph_tools_library_directory', NULL), $whitelist, $nomask = array('.', '..', 'jquery.js', 'CVS', 'fckeditor', 'tinymce'));
    $form  = array();
    $weight_options = array();
    for ($i = 0; $i < count($files); $i++) {
      $weight_options[$i] = $i;
    }
    if (!empty($files)) {
      //Reset our incremental var
      $i = 0;
      //If we find jquery libraries here, then allow them as settings parameters. We want to ignore directories with fck or tiny because those are reserved for the wysiwyg module.
      foreach ($files as $key => $file) {
        $options = array(
           0               => 'DISABLE',
           $file->filename => 'ENABLE',
        );
        $form['script_files'][$i] = array(
          '#type' 		=> 'fieldset',
          '#title' 		=> t('@name', array('@name' => $file->name)),
          '#collapsible' 	=> TRUE,
          '#collapsed' 	=> TRUE,
          '#weight'		=> variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_weight', $i),
          '#group'        => 'jquery_plugins',
          '#prefix' => '<li id="phtools-sortable-'.$i.'">',
          '#suffix' => '</li>'
        );
        $form['script_files'][$i]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)] = array(
          '#title' 		   => t('Enable or Disable @name', array('@name' => $file->name)),
          '#description'   => t('This file is located at: @dir. Change the weight to re-order the weight. These scripts will always come after contributed modules.', array('@dir' => $file->filename)),
          '#type' 		   => 'radios',
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name), 0),
          '#options' 	   => $options,
          '#weight'        => 0,
          '#group'         => 'jquery_plugins',
          '#prefix'        => '<hr />',
        );
        //Allows us to set the scope if we are enabled
        $scope = array('header' => t('Header'), 'footer' => t('Footer'));
        $form['script_files'][$i]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_scope'] = array(
          '#title' 				 => t('Scope'),
          '#description' 	 => t('Set the Scope for this script for the header or the footer of the page.'),
          '#type' 				 => 'radios',
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_scope', 'header'),
          '#options' 			 => $scope,
          '#weight'        => 9,
          '#group'         => 'jquery_plugins',
        );
        //Paths disallowed. @todo - make this work like the contexts module's paths.
        $form['script_files'][$i]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_paths'] = array(
          '#title' 				 => t('Paths to exclude this script from.'),
          '#description' 	 => t('Set the pages in which you do not wish to have this script run.'),
          '#type' 				 => 'textarea',
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_paths', variable_get('ph_tools_global_paths', PH_TOOLS_MODULE_GLOBAL_PATHS)),
          '#weight'        => 10,
          '#group'         => 'jquery_plugins',
        );
        //Weight for each script.
        $form['script_files'][$i]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_weight'] = array(
          '#title' 				 => t('Weight for this script.'),
          '#description' 	 => t('Set the weight for this script.'),
          '#type' 				 => 'hidden',
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_weight', $i),
          '#weight'        => 11,
          '#group'         => 'jquery_plugins',
          '#attributes'    => array(
            'class' => 'form-weight',
          ),
        );
        $i++;
      }
    }
    return $form;
  }
  
  /**
   * @name _checkPath();
   * Checks for TRUE/FALSE if we can instantiate a jquery plug-in
   * @todo - make me work like the context module.
   * @return bool true/false
   */
  public static function _check_path($paths = NULL) {
    if ($paths != NULL) {
      $path 	= drupal_get_path_alias($_GET['q']);
      $regexp = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1'. preg_quote(variable_get('site_frontpage', 'node'), '/') .'\2'), preg_quote($paths, '/')) .')$/';
      $found 	= preg_match($regexp, $path);
      if ($found == 1) {
        return FALSE;
      }
    }
    //Else we have a match, then we can invoke our caller.
    return TRUE;
  }
  
  /**
   * @name _ph_tools_load_js()
   * Load Jquery, Jquery UI and Preload images via the admin settings
   * @todo - make settings a $settings array within the phTools namespace.
   */
  public static function _ph_tools_load_js() {
    //Global settings, this is for the main settings of the module.
    if (self::_check_path(variable_get('ph_tools_global_paths',PH_TOOLS_MODULE_GLOBAL_PATHS))) {
      //Add Path To Theme setting, so you can get the path to theme.
      $current_theme  = variable_get('theme_default','none');
      $themes         = list_themes();
      $theme_object   = $themes[$current_theme];
      $settings['phTools']['getPathToTheme'] = drupal_get_path('theme', $theme_object->name);
      //Extended Drupal JS functions, under the Drupal.PHtools.%function
      drupal_add_js(PH_TOOLS_MODULE_PATH. '/js/phtools.drupal.functions.js','module','header');
      //Add Destination Setting, so you can have the drupal_get_destination() as a js available Drupal.settings.
      $settings['phTools']['getDestination'] = drupal_get_destination();
      //Add in search indexing autco-complete.
      $settings['phTools']['searchIndex']    = drupal_to_js(variable_get('ph_tools_search_index',array('PH-Balanced')));
      drupal_add_js($settings, 'setting');
      //Add Fade Functionality.
      self::_message_can_fade();
    }
    //Loop through each library setting and add the file information if we have a valid page.
    $library = variable_get('ph_tools_library', array());
    ksort($library);
    if (!empty($library)) {
      foreach ($library as $key => $script) {
        $path  = drupal_get_path_alias($_GET['q']);
        $paths = preg_split('/[\r\n]+/', $script['paths']);
        if (!self::_match_path($path, $paths, TRUE)) {
          drupal_add_js($script['script'], 'theme', $script['scope']);
        }
      }
    }
  }
  
  /**
   * @name _message_can_fade()
   * Find out if the path given is valid for fading out messagess.
   */
  public static function _message_can_fade() {
    //Settings for fade out delay.
    $settings['phTools']['fade']['enabled'] = variable_get('phtools_fade_messages_override', 0);
    $settings['phTools']['fade']['delay']   = variable_get('ph_tools_fade_out_delay', 5500);
    //Add these to Drupal.settings
    drupal_add_js($settings, 'setting');
  }

  /**
   * @internal get the search index into an array we can use for keys in the autocomplete search box.
   */
  public static function _setSearchIndex() {
    $index = array();
    $data  = db_query("SELECT word FROM {search_index} WHERE type = 'node' GROUP BY word ORDER BY word ASC");
    while ($rs = db_fetch_array($data)) {
      $word = trim(strtolower($rs['word']));
      $index[] = $word;
    }
    variable_set('ph_tools_search_index',$index);
    watchdog('PH-Tools', t('Search Index Complete.'));
  }

  /**
   * Callback Helper for all AHAH propagated by phTools.
   */
  public static function _ahahCallbackHelper($type = NULL) {
    //Switch between types.
    switch ($type) {
      case 'theme-settings':
        //Get the last element, so we can update the id and name.
        //@todo - I think this needs to be audited?
        //print '<pre></pre>';
        //print_r($_POST);
        $items = 0;
        foreach ($form['theme_specific']['theme_settings']['meta_tags']['wrapper'] as $key => $value) {
          if (strstr($key, 'element_meta_tags')) {
            $items++;
          }
        }
        //kpr($form['theme_specific']['theme_settings']['meta_tags']['wrapper']);
        $item = $form['theme_specific']['theme_settings']['meta_tags']['wrapper'];
        //kpr($item);
        //Remove the prefix/suffix wrapper so we don't double it up.
        unset($item['#prefix'], $item['#suffix']);
        //Create output with error messages and new item. @internal - if there are submission problems, it's because the _submit function from
        //the theme_settings is firing un-intentionally.
        $output = theme('status_messages');
        $output .= _ahahRender($item);
        break;
    }
    return $output;
  }

  /**
   * Callback Helper for all AHAH propagated by phTools.
   */
  public static function _ahahCallbackRender($fields, $name) {
    $form_state = array('submitted' => FALSE);
    $form_build_id = $_POST['form_build_id'];
    // Add the new element to the stored form. Without adding the element to the
    // form, Drupal is not aware of this new elements existence and will not
    // process it. We retreive the cached form, add the element, and resave.
    $form = form_get_cache($form_build_id, $form_state);
    $form[$name] = $fields;
    form_set_cache($form_build_id, $form, $form_state);
    $form += array(
      '#post' => $_POST,
      '#programmed' => FALSE,
    );
    // Rebuild the form.
    $form = form_builder($_POST['form_id'], $form, $form_state);
    //Render the new output.
    $new_form = $form[$name];
    return drupal_render($new_form);
  }
  
  /**
   * Match the subject against a set of regex patterns.
   * Similar to drupal_match_path() but also handles negation through the use
   * of the ~ character.
   * 
   * Shamelessly stolen from the context module to make my life eaier.
   *
   * @param mixed $subject
   *   The subject string or an array of strings to be matched.
   * @param array $patterns
   *   An array of patterns. Any patterns that begin with ~ are considered
   *   negative or excluded conditions.
   * @param boolean $path
   *   Whether the given subject should be matched as a Drupal path. If TRUE,
   *   '<front>' will be replaced with the site frontpage when matching against
   *   $patterns.
   */
  public static function _match_path($subject, $patterns, $path = FALSE) {
    static $regexps;
    $match = FALSE;
    $positives = $negatives = 0;
    $subject = !is_array($subject) ? array($subject) : $subject;
    foreach ($patterns as $pattern) {
      if (strpos($pattern, '~') === 0) {
        $negate = TRUE;
        $negatives++;
      }
      else {
        $negate = FALSE;
        $positives++;
      }
      $pattern = ltrim($pattern, '~');
      if (!isset($regexps[$pattern])) {
        if ($path) {
          $regexps[$pattern] = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1'. preg_quote(variable_get('site_frontpage', 'node'), '/') .'\2'), preg_quote($pattern, '/')) .')$/';
        }
        else {
          $regexps[$pattern] = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/'), array('|', '.*'), preg_quote($pattern, '/')) .')$/';
        }
      }
      foreach ($subject as $value) {
        if (preg_match($regexps[$pattern], $value)) {
          if ($negate) {
            return FALSE;
          }
          $match = TRUE;
        }
      }
    }
    // If there are **only** negative conditions and we've gotten this far none
    // we actually have a match.
    if ($positives === 0 && $negatives) {
      return TRUE;
    }
    return $match;
  }
//end.
}

/**
 * phToolsAdmin
 * This houses the theme-settings.php $form
 */
class phToolsThemeAdmin {

  /**
   * @name _getAdminForm($form = array());
   * @param <type> $form
   * @return <type>
   */
  public static function _getAdminForm($saved_settings, $theme_name, $info_theme_settings) {
    // Get current list of modules from the current cache.
    $modules = module_rebuild_cache();
    uasort($modules, 'system_sort_modules_by_info_name');
    //UI Buttons.
    $buttons = array(
      '!add_button'    => '<div class="btn-add-me"><button type="button" class="add-me" title="Add" alt="Add A New Row.">Add</button><p>Add A New Row</p></div>',
      '!delete_button' => '<button type="button" class="delete-me" title="Delete" alt="Delete this Row.">Delete</button>',
    );
    //Toggle UI. This does not necessarily need to be here, but is kinda hard to place inthe code.
    $toggle_options['attributes'] = array('class' => 'settings-toggle all-on');
    $toggle_link = l(t('Toggle All On'), $_GET['q'], $toggle_options);
    //Form settings parameters.
    $form['#tree'] = TRUE;
    $form['theme_settings'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Theme Settings & Optimizations', array('@name' => $theme_name)),
      '#description' => t('Use this theme to set your theme parameters.'),
      '#collapsible' => TRUE,
      '#collapsed'   => FALSE,
    );
    //Meta Tags.
    $form['theme_settings']['meta_tags'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Meta tags'),
      '#description' => t('Add, modify or delete Meta tags for this theme.'),
      '#collapsible' => TRUE,
      '#collapsed'   => FALSE,
      '#attributes' => array(
        'id' => 'fieldset-meta-tags',
      ),
    );
    //Add Meta tag Button.
    $form['theme_settings']['meta_tags']['add'] = array(
      '#type' => 'submit',
      '#value' => t('Add New Meta Tag'),
      '#name' => 'add-me',
      '#attributes' => array(
        'class' => 'add-me',
      ),
      '#ahah' => array(
        'path' => 'admin/settings/ph_tools/ahah/theme-settings',
        'wrapper' => 'meta-tags-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
      ),
      '#prefix' => '<div class="btn-add-me">',
      '#suffix' => '</div>',
    );
    //Meta Tags Wrapper.
    $form['theme_settings']['meta_tags']['wrapper'] = array(
      '#prefix' => '<div id="meta-tags-wrapper" class="dynamic-element-wrapper">',
      '#suffix' => '</div>',
    );
    //Meta Tags can be added here. If there are no saved settings for this option, only one form field displays.
    $meta_tags = array(
      0 => array(
        'meta_name'    => 'ICBM',
        'meta_content' => '',
      ),
      1 => array(
        'meta_name'    => 'geo.position',
        'meta_content' => '',
      ),
      2 => array(
        'meta_name'    => 'geo.placename',
        'meta_content' => '',
      ),
      3 => array(
        'meta_name'    => 'geo.region',
        'meta_content' => '',
      ),
    );
    $saved_settings['intercept_meta_tags'] = (empty($saved_settings['intercept_meta_tags'])) ? $meta_tags : $saved_settings['intercept_meta_tags'];
    foreach ($saved_settings['intercept_meta_tags'] as $key => $value) {
      if (!empty($value['meta_name'])) {
        //If we have both a name, apply the meta tag.
        $form['theme_settings']['meta_tags']['wrapper']['element_meta_tags_' .$key] = array(
          '#type'          => 'theme_settings_form_meta_tags',
          '#default_value' => array(
            'meta_name'    => $value['meta_name'],
            'meta_content' => $value['meta_content'],
          ),
          '#element_validate' => array('_phtools_theme_settings_validate'),
        );
      }
    }
    //Stylesheet aggregation.
    $form['theme_settings']['intercept_styles'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Theme Stylesheet Settings'),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => FALSE,
    );
    //Stylesheet settings. You can turn on or turn off javascript interception, and also set what javascript you want to ignore.
    $form['theme_settings']['intercept_styles']['intercept_styles'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Aggregate and cache stylesheets from $vars[\'styles\'] on a per page load basis. All stylesheets are enabled by default. Un-Check a specific checkbox to omit that file form being loaded.'),
      '#default_value' => $saved_settings['intercept_styles'],
      '#description'   => t('By checking this box, you will intercept the $vars[\'scripts\'] variable at page load, if there is no cache file present for this page, or the hash for this file changes, then the module will update that file on the fly and re-cache a leaner agggregated file based on page load.'),
      '#prefix'        => '<h2>Stylesheet Aggregation</h2>',
    );
    /**
     * Stylesheet file exclusion
     * This denies any stylesheet we do not want to load to the page via theme UI.
     * @todo - see javascript todo you are repeating yourself here, lazy git.
     */
    //Misc Drupal Files.
    $misc_files   = file_scan_directory('misc', '.css');
    $form['theme_settings']['intercept_styles']['misc'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Misc Styleshseet Files. @count files', array('@count' => count($misc_files))),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );
    //Reset the files array.
    $files = array();
    foreach ($misc_files as $key => $file) {
      $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
    }
    $form['theme_settings']['intercept_styles']['misc']['element_styles_excludes_misc'] = array(
      '#type'          => 'checkboxes',
      '#options'       => $files,
      '#default_value' => !empty($saved_settings['element_styles_excludes_misc']) ? $saved_settings['element_styles_excludes_misc'] : array_keys($files),
    );
    //Sacn Library js files if the directory exists.
    $dir = 'sites/all/libraries';
    $success = file_check_directory($dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
    if ($success) {
      $libraries_files   = file_scan_directory('sites/all/libraries', '.css');
      //kpr($libraries_files);
      $form['theme_settings']['intercept_styles']['libraries'] = array(
        '#type'        => 'fieldset',
        '#title'       => t('Library Files. @count files', array('@count' => count($libraries_files))),
        '#description' => t(''),
        '#collapsible' => TRUE,
        '#collapsed'   => TRUE,
      );
      //Reset the files array.
      $files = array();
      foreach ($libraries_files as $key => $file) {
        $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
      }
      $form['theme_settings']['intercept_styles']['libraries']['element_styles_excludes_libraries'] = array(
        '#type'          => 'checkboxes',
        '#options'       => $files,
        '#default_value' => !empty($saved_settings['element_styles_excludes_libraries']) ? $saved_settings['element_styles_excludes_libraries'] : array_keys($files),
      );
    }
    //Module js files.
    $form['theme_settings']['intercept_styles']['modules'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Module Stylesheet Files.'),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );
    //Loop Through Modules.
    foreach ($modules as $name => $module) {
      //If a module is active, scan the directory for each javscript file and make that into a disable list.
      if ($module->status == 1 && $module->type == 'module') {
        //Scan for js files.
        $path = drupal_get_path('module', $module->name);
        $module_files = file_scan_directory($path, '.css');
        if (count($module_files) > 0) {
          //Make a module fieldset
          $form['theme_settings']['intercept_styles']['modules'][$name] = array(
            '#type'        => 'fieldset',
            '#title'       => t('@name @count files', array('@name' => $module->info['name'], '@count' => count($module_files))),
            '#description' => t('@description', array('@description' => $module->info['description'])),
            '#collapsible' => TRUE,
            '#collapsed'   => TRUE,
          );
          //Toggle All.
          $form['theme_settings']['intercept_styles']['modules'][$name][$name .'_toggle'] = array(
            '#type'       => 'markup',
            '#value'      => $toggle_link,
            '#weight'     => -100,
          );
          //Reset the files array.
          $files = array();
          //Populate module files.
          foreach ($module_files as $key => $file) {
            $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
          }
          $form['theme_settings']['intercept_styles']['modules'][$name]['element_styles_excludes_modules_'.$name] = array(
            '#type'          => 'checkboxes',
            '#options'       => $files,
            '#default_value' => !empty($saved_settings['element_styles_excludes_modules_'.$name]) ? $saved_settings['element_styles_excludes_modules_'.$name] : array_keys($files),
          );
          //Unset our files.
          unset($files);
        }
        //Unset our module files
        unset($module_files);
      }
    }
    //Toggle Link for all remaining directories.
    $form['theme_settings']['intercept_styles']['libraries']['libraries_toggle'] =
    $form['theme_settings']['intercept_styles']['misc']['misc_toggle'] = array(
      '#type'       => 'markup',
      '#value'      => $toggle_link,
      '#weight'     => -100,
    );
    //Javascript Aggregation.
    $form['theme_settings']['intercept_js'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Theme Javascript Settings'),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => FALSE,
    );
    //Javascript settings. You can turn on or turn off javascript interception, and also set what javascript you want to ignore.
    $form['theme_settings']['intercept_js']['intercept_js'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Aggregate Javascript from $vars[\'scripts\'] and remove unwanted scripts from this theme. All javascript is enabled by default. Un-Check a specific checkbox to omit that file form being loaded.'),
      '#default_value' => $saved_settings['intercept_js'],
      '#description'   => t('By checking this box, you will intercept the $vars[\'scripts\'] variable, remove unwanted javascript, aggregate it and return the variable $vars[\'intercept_scripts\']. This has the effect of making one js request in the header, but also optimizes the javascript to be leaner for mobile browsers. NOTE: this onnly works for javascript set in the header.'),
      '#prefix'        => '<h2>Javascript Aggregation</h2>',
    );
    /**
     * Javascript file exclusion
     * This denies any javascript we do not want to load to the page via theme UI.
     * @todo - I know this code can get compacted, I just wanted ot make sure I could suss out how it works.
     */
    //Core js files.
    $core_files   = file_scan_directory('modules', '.js', array('.', '..', 'CVS', 'drupal.js', 'jquery.js'));
    //kpr($core_files);
    $form['theme_settings']['intercept_js']['core'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Core Javascript Files. @count files', array('@count' => count($core_files))),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );
    //Reset the files array.
    $files = array();
    foreach ($core_files as $key => $file) {
      $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
    }
    $form['theme_settings']['intercept_js']['core']['element_js_excludes_core'] = array(
      '#type'          => 'checkboxes',
      '#options'       => $files,
      '#default_value' => !empty($saved_settings['element_js_excludes_core']) ? $saved_settings['element_js_excludes_core'] : array_keys($files),
    );
    //Misc Drupal Files.
    $misc_files   = file_scan_directory('misc', '.js');
    $form['theme_settings']['intercept_js']['misc'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Misc Javascript Files. @count files', array('@count' => count($misc_files))),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );
    //Reset the files array.
    $files = array();
    foreach ($misc_files as $key => $file) {
      $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
    }
    $form['theme_settings']['intercept_js']['misc']['element_js_excludes_misc'] = array(
      '#type'          => 'checkboxes',
      '#options'       => $files,
      '#default_value' => !empty($saved_settings['element_js_excludes_misc']) ? $saved_settings['element_js_excludes_misc'] : array_keys($files),
    );
    //Sacn Library js files if the directory exists.
    $dir = 'sites/all/libraries';
    $success = file_check_directory($dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
    if ($success) {
      $libraries_files   = file_scan_directory('sites/all/libraries', '.js');
      //kpr($libraries_files);
      $form['theme_settings']['intercept_js']['libraries'] = array(
        '#type'        => 'fieldset',
        '#title'       => t('Library Files. @count files', array('@count' => count($libraries_files))),
        '#description' => t(''),
        '#collapsible' => TRUE,
        '#collapsed'   => TRUE,
      );
      //Reset the files array.
      $files = array();
      foreach ($libraries_files as $key => $file) {
        $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
      }
      $form['theme_settings']['intercept_js']['libraries']['element_js_excludes_libraries'] = array(
        '#type'          => 'checkboxes',
        '#options'       => $files,
        '#default_value' => !empty($saved_settings['element_js_excludes_libraries']) ? $saved_settings['element_js_excludes_libraries'] : array_keys($files),
      );
    }
    //Module js files.
    $form['theme_settings']['intercept_js']['modules'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Module Javascript Files.'),
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );
    //Loop Through Modules.
    foreach ($modules as $name => $module) {
      //If a module is active, scan the directory for each javscript file and make that into a disable list.
      if ($module->status == 1 && $module->type == 'module') {
        //Scan for js files.
        $path = drupal_get_path('module', $module->name);
        $module_files = file_scan_directory($path, '.js');
        if (count($module_files) > 0) {
          //Make a module fieldset
          $form['theme_settings']['intercept_js']['modules'][$name] = array(
            '#type'        => 'fieldset',
            '#title'       => t('@name @count files', array('@name' => $module->info['name'], '@count' => count($module_files))),
            '#description' => t('@description', array('@description' => $module->info['description'])),
            '#collapsible' => TRUE,
            '#collapsed'   => TRUE,
          );
          //Reset the files array.
          $files = array();
          //Populate module files.
          foreach ($module_files as $key => $file) {
            $files[$file->filename] = t('@name - @file', array('@name' => $file->name, '@file' => $file->filename));
          }
          $form['theme_settings']['intercept_js']['modules'][$name]['element_js_excludes_modules_'.$name] = array(
            '#type'          => 'checkboxes',
            '#options'       => $files,
            '#default_value' => !empty($saved_settings['element_js_excludes_modules_'.$name]) ? $saved_settings['element_js_excludes_modules_'.$name] : array_keys($files),
          );
          //Unset our files.
          unset($files);
        }
        //Unset our module files
        unset($module_files);
      }
    }
    //Toggle Link for all remaining directories.
    $form['theme_settings']['intercept_js']['libraries']['libraries_toggle'] =
    $form['theme_settings']['intercept_js']['misc']['misc_toggle'] =
    $form['theme_settings']['intercept_js']['core']['core_toggle'] = array(
      '#type'       => 'markup',
      '#value'      => $toggle_link,
      '#weight'     => -100,
    );
    //Add in our own submit handler.
    $form['#validate'][] = '_phtools_theme_settings_validate';
    //Return our form.
    return $form;
  }
}