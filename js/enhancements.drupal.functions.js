//Drupal Settings, access as Drupal.settings.[function]
if (Drupal.jsEnabled) {
	
  // Make sure our objects are defined for theme objects.
  Drupal.CoreEnhancements 			= Drupal.CoreEnhancements || {};
  Drupal.CoreEnhancements.theme = Drupal.CoreEnhancements.theme || {};
	
	//Drupal Trace, makes it so that console is invoked only on compatible browsers
	Drupal.trace = function( string ) {
      
      if ( window.console ) {
				
        if ( console.debug ) {
					
          console.debug ( string );
					
        } else if ( console.log ) {
					
          console.log ( string );
					
        }
				
      }
      
	}
	
	//Create a link, emulates drupal l() function
	//var atr = {}
	//attr.classes = 'my-class one my-class-two my-class-three';
	//attr.rel ='my-rel-here';
	//attr.alt = 'my-alt-tags-here';
	// Drupa.l( 'My Link', '/mylink/url', attr, Drupal.settings.drupal_get_destination, 'my-link' );
  Drupal.l = function ( title, ref, attr, destination, id )  {
      
      var output = '<a id="'+id+'" alt"'+attr.alt+'" rel="'+attr.rel+'" name="'+id+'" class="'+attr.classes+'" href="'+ref+'?'+Drupal.urlDecode(destination)+'">'+Drupal.t( title )+'</a>';
      return output;
      
	}
	
	//serialize data
	Drupal.serialize = function ( data, prefix ) {
      
      prefix = prefix || '';
      var out = '';
      for (i in data) {
          var name = prefix.length ? (prefix +'[' + i +']') : i;
          if (out.length) out += '&';
          if (typeof data[i] == 'object') {
              out += Drupal.serialize(data[i], name);
          }
          else {
              out += name +'=';
              out += Drupal.encodeURIComponent(data[i]);
          }
      }
      return out;
      
	}
	
	//URL decode, extends Drupal.js
	Drupal.urlDecode = function ( encodedString ) {
      
      var output = encodedString;
      var binVal, thisString;
      var myregexp = /(%[^%]{2})/;
			
      while ((match = myregexp.exec(output)) != null
        && match.length > 1
        && match[1] != '') {
          binVal      = parseInt(match[1].substr(1),16);
          thisString  = String.fromCharCode(binVal);
          output      = output.replace(match[1], thisString);
        }
        
      return output;
      
	}
    
  //save cookie
	Drupal.saveCookie = function ( name, data, discontinues, domain ) {
      
		if ( name == null || name == undefined ) { return false; }
		if ( data == null || data == null ) { return false; }
		if ( discontinues == null ) { discontunies = 0; }
		if ( domain == null || domain == undefined ) { domain = '/'; }
		
		deleteCookie( name ); //delete the cookie so we know we always have a fresh cookie
		$.cookie(name, data, {
			path:			domain,
			expires: 	discontinues
		});
		
		return true;
        
	}
	
  //delete cookie
	Drupal.deleteCookie = function ( name ) {
      
      $.cookie(type, null);
        
	}
    
	//scroll to selected element
	Drupal.scrollTo = function (target) {
		
		//Drupal.trace(target);
		$('html, body').animate({ scrollTop: $(target).offset().top }, 1000);
		
	}
    
  //Validate form validates all form fields on a form in a specific div tag
	Drupal.validateForm = function ( formTarget, pageTarget, showSubmit ) {
      
      var success   = true;
      var fields 		= {};
      var result		= {};
      
      $("#drupal-elements").html('');

      Drupal.scrollTo( pageTarget ); //scroll to where you want it to go, can be a class or an id
      
      $("input, select, textarea").each( function() {
          
        $(this).removeClass('error');
				
				//validate email
				if ( $(this).attr("id").match( /email/ ) && $(this).parents().is(formTarget) && $(this).val() ) {
					
					var pattern = new RegExp(/^(("[\w-\s]+")|([\w-]+(?:\.[\w-]+)*)|("[\w-\s]+")([\w-]+(?:\.[\w-]+)*))(@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$)|(@\[?((25[0-5]\.|2[0-4][0-9]\.|1[0-9]{2}\.|[0-9]{1,2}\.))((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\.){2}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\]?$)/i);
					
					success = pattern.test( $(this).val() );
					( success == false ) ? $(this).addClass('error') : '';
					
				}
				
				//validate null values
        if ( !$(this).val() && $(this).hasClass('required') && $(this).parents().is(formTarget) || !$(this).val() && !$("input[@id="+$(this).attr('id')+"]:checked").val() && $(this).hasClass('required') && $(this).parents().is(formTarget) || !$(this).val() && !$("input[@id="+$(this).attr('id')+"]:checked").val('NULL') && $(this).hasClass('required') && $(this).parents().is(formTarget) ) {
          
					//If the form item value is empty, and required flag it with the error class, and turn success to false
          success = false;
          $(this).addClass('error');
            
        } else if ( $(this).val() && $(this).parents().is(formTarget) || $("input[@id="+$(this).attr('id')+"]:checked").val() && $(this).parents().is(formTarget) ) {
            
          var fieldVal = '';
          
					//If the field set is true, return that fields value
          if ($(this).attr('type') == 'checkbox' || $(this).attr('type') == 'radio') {
						
            fieldVal = $("input[@id="+$(this).attr('id')+"]:checked").val() ? $("input[@id="+$(this).attr('id')+"]:checked").val() : null;
						
          } else {
						
            fieldVal = $("input[@id="+$(this).attr('id')+"]:checked").val() ? $("input[@id="+$(this).attr('id')+"]:checked").val() : $(this).val();
						
          }
          
          fields[$(this).attr('id')] = fieldVal;
            
        }
          
      });
      
      if ( success == false ) {
				
        $("#drupal-elements").html('<div class="error">Sorry, but you must fill out all the required for items, please try again.</div>');
				
      }
      
			//In case the submit hide module is used, show the submit button again
      $(showSubmit).show();
      $(".hide_submit").remove();
      
			//return true/false and the fields that have been processed
      result.success = success;
      result.fields = fields;
			
      return result;
		
	}
	
	/**
	 * Theme elements
	 */
	
	//This function allows multiple progress bars in different divs to run at the same time
	//This works with premade div ids only, so you have to make the div first, then theme the progress
	//Call this from Drupal.theme( 'progress', target, j, total, startMessage, endMessage )
	Drupal.theme.prototype.progress = function ( target, j, total, startMessage, endMessage ) {
      
      //Drupal.trace(total);
      //Drupal.trace(j);
      var message 	= $("div[@id='"+target+"']");
      var percent 	= Math.round((parseInt(j)+1)/parseInt(total)*100);
      message.html('');
      message.show();
      //Drupal.trace(percent);
      if ( percent < 100 ) {
        message.html( Drupal.CoreEnhancements.theme.progressBar( target, percent, startMessage ) );
      } else if ( percent >= 100 ) {
        percent = 100;
        message.html( Drupal.CoreEnhancements.theme.progressBar( target, percent, endMessage) );
        setTimeout( function() { message.fadeOut('slow'); } , 2000);
      }
        
	}
	
	//Progress Bar - html for Drupal.theme( 'progress' );
	Drupal.CoreEnhancements.theme.progressBar = function ( target, percent, message ) {
      
      var output;
      output = '<div id="progress-'+target+'" class="progress">';
      output += '<div class="bar"><div class="filled" style="width: '+ percent +'%"></div></div>';
      output += '<div class="percentage">'+ percent +'%</div>';
      output += '<div class="message">'+ Drupal.t( message ) +'</div>';
      output += '</div>';
  
      return output;
      
	}

}