<?php
/**
 * Widgets
 * Add Widgets here, go for it!
 */

/**
 * Weather Widget
 */
function _get_weather_widget( $url ) {
	
	if ( !empty( $url ) && strstr( $url, 'http://www.weather.gov/xml/current_obs' ) ) { //check to see if the url is valid
		
		$url = $url . '?nocache=' .md5( time() ); //make sure the xml does not cache
		$xml = simplexml_load_file ( $url );
		
		//print_r( $xml );
		
		$widget = new stdClass;
		
		foreach ( $xml as $key => $value ) {
			
			$widget->$key = (string) trim( $value );
			
		}
		
		//print_r( $widget );
		
		theme( 'weather_widget', $widget );
		
	} else {
		
		drupal_set_message( t( 'You have not entered in a valid url, please enter in a url destination from http://www.weather.gov/xml/current_obs' ), 'error' );
		
	}
	
}

//NOAA really hates consistency, so we have to make sure that each item exists, before we render it
function theme_weather_widget ( $widget ) {
				
  //print_r( $widget ) ;
	
	$item = '';
	
	$item = '<div class="weather-widget" />';
	
    $item .= '<div class="weather-icon" />';
    
    if ( $widget->icon_url_base && $widget->icon_url_name )  {
    
      $item .= '<img src="' .$widget->icon_url_base .$widget->icon_url_name .'" />';
    
    }
    
    $item .= '</div>';
    
    $item .= '<div class="weather-info" >';
    
      if ( $widget->weather ) {
        
        $item .= '<div class="weather-weather" >' .t ( $widget->weather ) .'</div>';        
        
      }
    
      if ( $widget->temperature_string ) {
        
        $item .= '<div class="weather-temp" >' .t( $widget->temperature_string ) .'</div>';
      
      }
      
      if ( $widget->wind_string ) {
        
        $item .= '<div class="weather-wind" >Wind ' .t( $widget->wind_string ) .'</div>';
      }
      
      if ( $widget->relative_humidity ) {
        
        $item .= '<div class="weather-humidity" ><h4>Humidity:</h4> ' .t( $widget->relative_humidity ) .'% (relative)</div>';
      
      }
      
      if ( $widget->dewpoint_string ) {
        
        $item .= '<div class="weather-dewpoint" ><h4>Dew Point:</h4> ' .t( $widget->dewpoint_string ) .'</div>';
      
      }
      
      if ( $widget->visibility_mi ) {
        
        $item .= '<div class="weather-visibility" ><h4>Visibility:</h4> ' .t( $widget->visibility_mi ) .' miles</div>';
      
      }
      
      if ( $widget->windchill_string ) {
        
        $item .= '<div class="weather-windchill" ><h4>Wind Chill:</h4> ' .t( $widget->windchill_string ) .'</div>';
      
      }
      
      //Of course NOAA wil always keep this?
      $item .= '<div class="weather-observation-time" ><small>' .t( $widget->observation_time ) .'</small></div>';
      
      $item .= '<div class="weather-credit" ><small>' .t( $widget->credit ) .'</small></div>';
    
    $item .= '</div>';
	
	$item .= '</div>';
  
  print $item;
	
}