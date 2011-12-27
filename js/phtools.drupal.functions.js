//Drupal Settings, access as Drupal.settings.[function]
if (Drupal.jsEnabled) {
  var target, id, string, title, ref, attributes, query, fragment, data, prefix, name, discontinues, domain, success, regexp;
  var i, total, startMessage, endMessage, fade, output, percent, message, binVal, BrowserDetect, dataString, dataProp, index;
  //Make sure our objects are defined for theme objects.
  Drupal.phTools = Drupal.phTools || {};
  Drupal.phTools.theme = Drupal.phTools.theme || {};
  //Initialize theme scripts.
  Drupal.behaviors.phToolsInit = function(context) {
    //Since console.log can be such a hassle in IE, this will check it for you so you can put it in somehwere and not worry about deleting it later.
    if (Drupal.trace == undefined) {
      Drupal.trace = function (string) {
        Drupal.phTools.trace(string);
      }
    }
    //Apply Fade if settings say so.
    if (Drupal.settings.phTools.fade.enabled == 1) {
      Drupal.phTools._setFade();
    }
  }
  
	/**
   * @name Drupal.phTools.trace
   * Allows us to use the console.log or console.debug between browsers without haveing to use a different set of code each time.
   * This helps us debug javascript easily between browsers, and will not Fail the page load in Internet Explorer.
   */
	Drupal.phTools.trace = function(string) {
    if (window.console) {
      if (console.log) {
        console.log(string);
      } else if (console.debug) {
        console.debug(string);
      }
    } 
	}
  
	/**
   * @name Drupal.phTools.l
   * Create a link, emulates drupal l() function.
   * @param string title = The title of the link, uses drupals t() function to make it safe.
   * @param string ref = the href you arelinking to. This is required and you must point it to an internal or external path, as this does not emulate Drupal's path look-up.
   * @param object attributes = The attributes you wish to add to this link.
   *  var attributes = {}
   *  attributes.classes = 'my-class one my-class-two my-class-three';
   *  attributes.rel     =' my-rel-here';
   *  attributes.alt     = 'my-alt-tags-here';
   *  attributes.title   = 'My Title Here';
   *  attributes.id      = 'my-id-here'; The id is also the name
   * @param array query     = an array of queries to add to the end of the href path
   *  var str = Drupal.settings.drupal_get_destination
   *  str = str.replace('destination=','');
   *  var query = {
   *  destination : str
   *  }
   * @param string fragment = the 'my-fragment' you wish to add. You do not need to use #
   * 
   * Example:
   * Drupal.phTools.l('My Link', '/mylink/url', attributes, query, fragment);
   *
   * @return null
   */
  Drupal.phTools.l = function (title, ref, attributes, query, fragment)  {
    //If we have a blank ref kill off the link.
    if (ref == null || ref == undefined) { return false; }
    //If we have a blank title, leave it blank.
    if (title == null || title == undefined) { title = ''; }
    //Make sure all attributes are blank, if there are null or undefined values
    if (attributes.classes == null || attributes.classes == undefined) { attributes.classes = ''; }
    if (attributes.rel == null || attributes.rel == undefined) { attributes.rel = ''; }
    if (attributes.alt == null || attributes.alt == undefined) { attributes.alt = ''; }
    if (attributes.title == null || attributes.title == undefined) { attributes.title = ''; }
    if (attributes.id == null || attributes.id == undefined) { attributes.id = ''; }
    if (attributes.name == null || attributes.name == undefined) { attributes.name = ''; }
    //If we have a query, render that into the href path.
    if (query != null || query != undefined) {
      i = 0;
      for (key in query) {
        if (i == 0) {
          ref += '?' +key +'=' +query[key];
        } else {
          ref += '&' +key +'=' +query[key];
        }
        i++;
      }
    }
    //If we have a framgent, then add that fragment.
    if (fragment == null || fragment == undefined) { fragment = ''; }
    if (fragment.length > 0) {
      ref = ref + '#' + fragment;
    }
    return '<a id="'+attributes.id+'" title="'+Drupal.t(attributes.title)+'" alt="'+Drupal.t(attributes.alt)+'" rel="'+attributes.rel+'" name="'+attributes.id+'" class="'+attributes.classes+'" href="'+ref+'">'+Drupal.t(title)+'</a>';
	}
	
	/**
   * @name Drupal.phTools.serialize
   * Allows us to serialize data on the fly with jscript, so we do not have to rely on only posting back to a drupal page.
   */
	Drupal.phTools.serialize = function (data, prefix) {
    prefix = prefix || '';
    output = '';
    for (i in data) {
      name = prefix.length ? (prefix +'[' + i +']') : i;
      if (output.length) output += '&';
      if (typeof data[i] == 'object') {
        output += Drupal.phTools.serialize(data[i], name);
      }
      else {
        output += name +'=';
        output += Drupal.phTools.encodeURIComponent(data[i]);
      }
    }
    return output;
	}
	
	/**
   * @name Drupal.phTools.urlDecode
   * Allows us to decode encoded strings with javascript
   */
	Drupal.phTools.urlDecode = function (string) {
    output = string;
    regexp = /(%[^%]{2})/;
    while ((match = regexp.exec(output)) != null && match.length > 1 && match[1] != '') {
      binVal = parseInt(match[1].substr(1),16);
      string = String.fromCharCode(binVal);
      output = output.replace(match[1], string);
    }
    return output;
	}
    
  /**
   * @name Drupal.phTools.saveCookie
   * Save a cookie using the jquery cookies plug-in - because sometimes cookies need to be done on the client side, like in mobile browsers.
   */
	Drupal.phTools.saveCookie = function (name, data, discontinues, domain) {
		if (name == null || name == undefined) { return false; }
		if (data == null || data == undefined) { return false; }
		if (discontinues == null || discontinues == undefined) { discontinues = 0; }
		if (domain == null || domain == undefined) { domain = '/'; }
		Drupal.phTools.deleteCookie(name); //delete the cookie so we know we always have a fresh cookie.
		$.cookie(name, data, {
			path:			domain,
			expires: 	discontinues
		});
		return true;
	}
	
  /**
   * @name Drupal.phTools.deleteCookie
   * Delete a cookie.
   */
	Drupal.phTools.deleteCookie = function (name) {
    $.cookie(name, null);
	}
    
	/**
   * @name Drupal.phTools.scrollTo
   * Scroll to a selected target, this allows us to control an offset and target more precisely.
   */
	Drupal.phTools.scrollTo = function (target, duration) {
    Drupal.phTools._scroll($(target).offset().top, duration);
	}
  
	Drupal.phTools._scroll = function (offset_top, duration) {
    if (offset_top >=0 ) {
      duration = duration || 1000;
      $('html, body').stop().animate({ scrollTop: offset_top }, duration);
    }
	}
    
  /**
   * @name Drupal.phTools.validateForm
   * Validate a form with javascript, if you want form validation that does not involve submitting the page.
   * This allows you target a div that has form items in it, and it will recursively scan the form for generic entry errors.
   * This looks for valid emails and null values.
   * On it's own, this will not stop the form from submitting, it will only validate, so you have to set your own success or fail depending on your form requirements.
   * Example Usage:
   *  $('#edit-submit, #edit-preview, li.vertical-tab-button a').bind('click',function() {
        var target = {};
        target.target       = 'fieldset.vertical-tabs-menu';
        target.vertical_tab = 'a.vertical-tabs-list-menu';
        return Drupal.phTools.validateForm(target);
      });
   * @param object target = the object you are sending to the validation routine
   * Example target:
   *  target.target = '.my-class-name' or '#my-id-name' or 'fieldset.vertical-tabs-name'
   *  target.verticalTab = 'a.vertical-tabs-list-menu'; This is the anchor tag so it points to the li parent, which is where the error class needs to be set.
   * @return boolean success = value true or false if succesful or not
   */
	Drupal.phTools.validateForm = function (target) {
    success = true;
    //Remove our vaidation message if we have one when we tart the validation sequence
    if ($("#validate-form-message").length > 0) {
      $("#validate-form-message").remove();
    }
    if ($(target.verticalTab).parent('li').length > 0) {
      $(target.verticalTab).parent('li').removeClass('error');
    }
    //If we have a valid target, run the validation
    if ($(target.target).length > 0) {
      //Find each input select or text area and validate that item if it is required 
      $(target.target).find('input, select, textarea').each(function() {
        //Remove the form error when we run the validation routine.
        $(this).removeClass('error');
        //Validate null values for textfields, textareas, select lists and checkboxes.
        if ($(this).attr('type') != 'checkbox' && $(this).hasClass('required') && !$(this).val() || $(this).attr('type') == 'checkbox' && $(this).hasClass('required') && !$(this).is(':checked')) {
          //If the form item value is empty, and required flag it with the error class, and turn success to false. This is a long set of logic.
          success = false;
          $(this).addClass('error');
          //Support For Vertical Tabs errors, if we assign a vertical tab identifier
          if ($(target.verticalTab).parent('li').length > 0) {
            $(target.verticalTab).parent('li').addClass('error');
          }
        }
        //Validate email if we have a field name *email*.
        if ($(this).attr('type') == 'text' && $(this).attr("id").match(/email/i) && $(this).val() && $(this).hasClass('required')) {
          regexp = new RegExp(/^(("[\w-\s]+")|([\w-]+(?:\.[\w-]+)*)|("[\w-\s]+")([\w-]+(?:\.[\w-]+)*))(@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$)|(@\[?((25[0-5]\.|2[0-4][0-9]\.|1[0-9]{2}\.|[0-9]{1,2}\.))((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\.){2}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\]?$)/i);
          success = regexp.test($(this).val());
          if (success == false) { $(this).addClass('error') };
        }
      });
      if (success == false) {
        //Set a container for our form messages.
        $(target.target).prepend('<div id="validate-form-message"></div>');
        //Set the error message to the validate-form-message we made.
        $("#validate-form-message").prepend('<div class="error"><p>Sorry, but you must fill out all the required form items, please try again. Each required field has an asterix (*) next to it.</p></div>');
      }
    }
    //Return true/false and the fields that have been processed.
    return success;
	}
  
  /**
   * @name Drupal.phTools.canFade
   * This allows us to fade objects with classes via drupal settings
   */
  Drupal.phTools._setFade = function () {
    if ($('.status, .warning').length > 0) {
      //Get the fading classes, and apply those fade-outs at load time.
      setTimeout(function(){
        //Drupal.trace('fading out');
        //Set the timeout for our classes.
        $('.status, .warning').fadeOut();
      }, Drupal.settings.phTools.fade.delay);
    }
  }
  
  /**
   * @name Drupal.phTools.browserDetect
   * Let's us detect a browser via javascript.
   * Browser Detect, this was taken from http://www.quirksmode.org/js/detect.html
   * BrowserDetect.browser
   * BrowserDetect.version
   * BrowserDetect.OS
   */
  Drupal.phTools.browserDetect = function () {
    BrowserDetect = {
      init: function () {
        this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
        this.version = this.searchVersion(navigator.userAgent)
          || this.searchVersion(navigator.appVersion)
          || "an unknown version";
        this.OS = this.searchString(this.dataOS) || "an unknown OS";
      },
      searchString: function (data) {
        for (i=0;i<data.length;i++)	{
          dataString = data[i].string;
          dataProp = data[i].prop;
          this.versionSearchString = data[i].versionSearch || data[i].identity;
          if (dataString) {
            if (dataString.indexOf(data[i].subString) != -1)
              return data[i].identity;
          }
          else if (dataProp)
            return data[i].identity;
        }
      },
      searchVersion: function (dataString) {
        index = dataString.indexOf(this.versionSearchString);
        if (index == -1) return;
        return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
      },
      dataBrowser: [
        {
          string: navigator.userAgent,
          subString: "Chrome",
          identity: "Chrome"
        },
        { 	string: navigator.userAgent,
          subString: "OmniWeb",
          versionSearch: "OmniWeb/",
          identity: "OmniWeb"
        },
        {
          string: navigator.vendor,
          subString: "Apple",
          identity: "Safari",
          versionSearch: "Version"
        },
        {
          prop: window.opera,
          identity: "Opera"
        },
        {
          string: navigator.vendor,
          subString: "iCab",
          identity: "iCab"
        },
        {
          string: navigator.vendor,
          subString: "KDE",
          identity: "Konqueror"
        },
        {
          string: navigator.userAgent,
          subString: "Firefox",
          identity: "Firefox"
        },
        {
          string: navigator.vendor,
          subString: "Camino",
          identity: "Camino"
        },
        {		// for newer Netscapes (6+)
          string: navigator.userAgent,
          subString: "Netscape",
          identity: "Netscape"
        },
        {
          string: navigator.userAgent,
          subString: "MSIE",
          identity: "Explorer",
          versionSearch: "MSIE"
        },
        {
          string: navigator.userAgent,
          subString: "Gecko",
          identity: "Mozilla",
          versionSearch: "rv"
        },
        { 		// for older Netscapes (4-)
          string: navigator.userAgent,
          subString: "Mozilla",
          identity: "Netscape",
          versionSearch: "Mozilla"
        }
      ],
      dataOS : [
        {
          string: navigator.platform,
          subString: "Win",
          identity: "Windows"
        },
        {
          string: navigator.platform,
          subString: "Mac",
          identity: "Mac"
        },
        {
             string: navigator.userAgent,
             subString: "iPhone",
             identity: "iPhone/iPod"
        },
        {
          string: navigator.platform,
          subString: "Linux",
          identity: "Linux"
        }
      ]
    
    };
    BrowserDetect.init();
    return BrowserDetect;
  }
	
	/**
	 * JS Theme Elements
	 */
	
	/**
   * @name = Drupal.theme.prototype.progress
   * Emulates the progress bar from drupal, without having to use AHAH, and you can use your own whatever to make this happen
   * Useage: Drupal.theme('progress', target, i, total, startMessage, endMessage, false);
   * @param string target = the name of the div you are targeting, you may use classes and ids, target anything you wish
   * @param integer i = the initial value to make a percentage from
   * @param integer total = the total, which calculates the percentage against i
   * @param string startMessage = the message to display when the percentage is less than 100%
   * @param string endMessage = the message to display when the percentage is 100%
   */
	Drupal.theme.prototype.progress = function (target, i, total, startMessage, endMessage, fade) {
    percent = '';
    if (fade == null || fade == undefined) { fade = false; }
    if (i >= 0 && total >= i) {
      percent = Math.round((parseInt(i) + 1) / parseInt(total) * 100);
      $("#theme-prototype-progress").remove();
      if (percent < 100) {
        $(target).prepend(Drupal.phTools.theme.progressBar(percent, startMessage));
      } else if (percent >= 100) {
        percent = 100;
        $(target).prepend(Drupal.phTools.theme.progressBar(percent, endMessage));
        if (fade != false) { //If fade is enabled
          setTimeout(function() { $("#theme-prototype-progress").fadeOut('slow'); } , 2000);
        }
      }
    }
    return false;   
	}
	
	/**
   * @name Drupal.phTools.theme.progressBar
   * Progress Bar - html wrapper for Drupal.theme('progress');
   */
	Drupal.phTools.theme.progressBar = function (percent, message) {
    output = '';
    if (percent && message) {
      output = '<div id="theme-prototype-progress" class="progress">';
      output += '<div class="bar"><div class="filled" style="width: '+ percent +'%"></div></div>';
      output += '<div class="percentage">'+ percent +'%</div>';
      output += '<div class="message">'+ Drupal.t(message) +'</div>';
      output += '</div>';
      return output;
    } else {
      output = 'No Progress Bar can be made, you need a percentage and a message, please try again';
      return false;
    }
	}
  //End Of File.
}