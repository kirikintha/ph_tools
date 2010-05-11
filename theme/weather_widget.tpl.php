<?php
/**
 * @param object $widget
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
?>
	
<div class="weather-widget">
	
  <div class="weather-icon" />
  
    <img src="<?php print $widget->icon_url_base .$widget->icon_url_name ?>" />
  
  </div>
    
  <div class="weather-info" >
  
    <div class="weather-weather" ><?php print t ( $widget->weather ) ?></div>        
    
    <div class="weather-temp" ><?php print t( $widget->temperature_string ) ?></div>
    
    <div class="weather-wind" >Wind <?php print t( $widget->wind_string ) ?></div>
    
    <div class="weather-humidity" ><h4>Humidity:</h4> <?php print t( $widget->relative_humidity ) ?>% (relative)</div>
    
    <div class="weather-dewpoint" ><h4>Dew Point:</h4> <?php print t( $widget->dewpoint_string ) ?></div>

    <div class="weather-visibility" ><h4>Visibility:</h4> <?php print t( $widget->visibility_mi ) ?> miles</div>
    
    <div class="weather-windchill" ><h4>Wind Chill:</h4> <?php print t( $widget->windchill_string ) ?></div>
      
    <div class="weather-observation-time" ><small><?php print t( $widget->observation_time ) ?></small></div>
    
    <div class="weather-credit" ><small><?php print t( $widget->credit ) ?></small></div>
  
  </div>
	
</div>