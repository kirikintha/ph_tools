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
		
		Drupal.deleteCookie( name ); //delete the cookie so we know we always have a fresh cookie
		$.cookie(name, data, {
			path:			domain,
			expires: 	discontinues
		});
		
		return true;
        
	}
	
  //delete cookie
	Drupal.deleteCookie = function ( name ) {
      
      $.cookie(name, null);
        
	}
    
	//scroll to selected element
	Drupal.scrollTo = function (target) {
		
		//Drupal.trace(target);
		$('html, body').animate({ scrollTop: $(target).offset().top }, 1000);
		
	}
    
  //Validate form validates all form fields on a form in a specific div tag
  /**
   * @param string formTarget = #your-div-id
   * @param string pagetTarget = #your-div-id
   * @param string showSubmit = #your-submit-button
   */
  var formTarget, pageTarget, showSubmit;
  formTarget = pageTarget = showSubmit = false;
	Drupal.validateForm = function ( formTarget, pageTarget, showSubmit ) {
      
      var success   = true;
      var fields 		= {};
      var result		= {};
      
      $("#drupal-elements").remove();
        
      $(formTarget).prepend('<div id="drupal-elements"></div>');

      if ( pageTarget ) {
        
        //-> @todo figure out why this is being so picky Drupal.scrollTo( pageTarget ); //scroll to where you want it to go, can be a class or an id
        
      }
      
      if ( formTarget ) {
        
        $("input, select, textarea").each( function() {
            
          $(this).removeClass('error');
          
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
          
          //validate email
          if ( $(this).attr("type") == 'text' && $(this).attr("id").match( /email/ ) && $(this).parents().is(formTarget) && $(this).val() ) {
            
            var pattern = new RegExp(/^(("[\w-\s]+")|([\w-]+(?:\.[\w-]+)*)|("[\w-\s]+")([\w-]+(?:\.[\w-]+)*))(@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$)|(@\[?((25[0-5]\.|2[0-4][0-9]\.|1[0-9]{2}\.|[0-9]{1,2}\.))((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\.){2}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\]?$)/i);
            
            success = pattern.test( $(this).val() );
            ( success == false ) ? $(this).addClass('error') : '';
            
          }
            
        });
        
        if ( success == false ) {
          
          $("#drupal-elements").prepend('<div class="error">Sorry, but you must fill out all the required form items, please try again.</div>');
          
        }
        
        if ( showSubmit ) {
          
          //In case the submit hide module is used, show the submit button again
          $(showSubmit).show();
          $(".hide_submit").remove();
          
        }
        
      }
      
      //return true/false and the fields that have been processed
      result.success = success;
      result.fields = fields;
      
      return result;
		
	}
	
	/**
	 * Theme elements
	 */
	
	/**
   * @name = Theme Progress
   * emulates the progress bar from drupal, without having to use AHAH, and you can use your own whatever to make this happen
   * Useage: Drupal.theme( 'progress', target, i, total, startMessage, endMessage, false );
   * @param string target = the name of the div you are targeting, you may use classes and ids, target anything you wish
   * @param integer i = the initial value to make a percentage from
   * @param integer total = the total, which calculates the percentage against i
   * @param string startMessage = the message to display when the percentage is less than 100%
   * @param string endMessage = the message to display when the percentage is 100%
   */
	Drupal.theme.prototype.progress = function ( target, i, total, startMessage, endMessage, fade ) {
    
    var percent;
    
    if ( fade == null || fade == undefined ) { fade = false; }
    
    if ( i >= 0 && total >= i ) {
      
      percent = Math.round( (parseInt( i ) + 1 ) / parseInt( total ) * 100 );
        
      $("#theme-prototype-progress").remove();
  
      if ( percent < 100 ) {
        
        $(target).prepend( Drupal.CoreEnhancements.theme.progressBar( percent, startMessage ) );
        
      } else if ( percent >= 100 ) {
        
        percent = 100;
        
        $(target).prepend( Drupal.CoreEnhancements.theme.progressBar( percent, endMessage ) );
        
        if ( fade != false ) { //If fade is enabled
          
          setTimeout( function() { $("#theme-prototype-progress").fadeOut('slow'); } , 2000 );
          
        }
        
      }
      
    }
    
    return false;
        
	}
	
	//Progress Bar - html for Drupal.theme( 'progress' );
	Drupal.CoreEnhancements.theme.progressBar = function ( percent, message ) {
      
    var output;
    
    if ( percent && message ) {
      
      output = '<div id="theme-prototype-progress" class="progress">';
      output += '<div class="bar"><div class="filled" style="width: '+ percent +'%"></div></div>';
      output += '<div class="percentage">'+ percent +'%</div>';
      output += '<div class="message">'+ Drupal.t( message ) +'</div>';
      output += '</div>';
  
      return output;
      
    } else {
      
      output = 'No Progress Bar can be made, you need a percentage and a message, please try again';
      
      return false;
      
    }
    

      
	}

}