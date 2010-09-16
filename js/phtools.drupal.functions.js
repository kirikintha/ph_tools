//Drupal Settings, access as Drupal.settings.[function]
if (Drupal.jsEnabled) {
  
  //Make sure our objects are defined for theme objects.
  Drupal.PHTools 			 = Drupal.PHTools || {};
  Drupal.PHTools.theme = Drupal.PHTools.theme || {};
  
	/**
   * @name Drupal.PHTools.trace
   * Allows us to use the console.log or console.debug between browsers without haveing to use a different set of code each time.
   * This helps us debug javascript easily between browsers, and will not Fail the page load in Internet Explorer.
   */
	Drupal.PHTools.trace = function(string) {
    if (window.console) {
      if (console.debug) {
        console.debug (string);
      } else if (console.log) {
        console.log (string);
      }
    } 
	}
  
	/**
   * @name Drupal.PHTools.l
   * Create a link, emulates drupal l() function
   * var attributes = {}
   * attributes.classes = 'my-class one my-class-two my-class-three';
   * attributes.rel     =' my-rel-here';
   * attributes.alt     = 'my-alt-tags-here';
   * attributes.title   = 'My Title Here'
   * Drupal.PHTools.l('My Link', '/mylink/url', attr, Drupal.settings.drupal_get_destination, 'my-link');
   */
  Drupal.PHTools.l = function (title, ref, attributes, destination, id)  {
    return output = '<a id="'+id+'" title="'+Drupal.t(attributes.title)+'" alt"'+Drupal.t(attributes.alt)+'" rel="'+attributes.rel+'" name="'+id+'" class="'+attrributes.classes+'" href="'+ref+'?'+Drupal.PHTools.urlDecode(destination)+'">'+Drupal.t(title)+'</a>';
	}
	
	/**
   * @name Drupal.PHTools.serialize
   * Allows us to serialize data on the fly with jscript, so we do not have to rely on only posting back to a drupal page.
   */
	Drupal.PHTools.serialize = function (data, prefix) {
    prefix = prefix || '';
    var out = '';
    for (i in data) {
      var name = prefix.length ? (prefix +'[' + i +']') : i;
      if (out.length) out += '&';
      if (typeof data[i] == 'object') {
        out += Drupal.PHTools.serialize(data[i], name);
      }
      else {
        out += name +'=';
        out += Drupal.PHTools.encodeURIComponent(data[i]);
      }
    }
    return out;
	}
	
	/**
   * @name Drupal.PHTools.urlDecode
   * Allows us to decode encoded strings with javascript
   */
	Drupal.PHTools.urlDecode = function (encodedString) {
    var output = encodedString;
    var binVal, thisString;
    var myregexp = /(%[^%]{2})/;
    while ((match = myregexp.exec(output)) != null && match.length > 1 && match[1] != '') {
      binVal      = parseInt(match[1].substr(1),16);
      thisString  = String.fromCharCode(binVal);
      output      = output.replace(match[1], thisString);
    }
    return output;
	}
    
  /**
   * @name Drupal.PHTools.saveCookie
   * Save a cookie using the jquery cookies plug-in - because sometimes cookies need to be done on the client side, like in mobile browsers.
   */
	Drupal.PHTools.saveCookie = function (name, data, discontinues, domain) {
		if (name == null || name == undefined) { return false; }
		if (data == null || data == undefined) { return false; }
		if (discontinues == null || discontinues == undefined) { discontunies = 0; }
		if (domain == null || domain == undefined) { domain = '/'; }
		Drupal.PHTools.deleteCookie(name); //delete the cookie so we know we always have a fresh cookie.
		$.cookie(name, data, {
			path:			domain,
			expires: 	discontinues
		});
		return true;
	}
	
  /**
   * @name Drupal.PHTools.deleteCookie
   * Delete a cookie.
   */
	Drupal.PHTools.deleteCookie = function (name) {
    $.cookie(name, null);
	}
    
	/**
   * @name Drupal.PHTools.scrollTo
   * Scroll to a selected target, this allows us to control an offset and target more precisely.
   */
	Drupal.PHTools.scrollTo = function (target) {
		$('html, body').animate({ scrollTop: $(target).offset().top }, 1000);
	}
    
  /**
   * @name Drupal.PHTools.validateForm
   * Validate a form with javascript, if you want form validation that does not involve submitting the page.
   * This allows you target a div that has a form in it, and it will recursively scan the form for generic entry errors.
   * This looks for valid emails and null values.
   * @param string formTarget  = #your-div-id
   * @param string pagetTarget = #your-div-id
   * @param string showSubmit  = #your-submit-button
   */
  var formTarget, pageTarget, showSubmit;
  formTarget = pageTarget = showSubmit = false;
	Drupal.PHTools.validateForm = function (formTarget, pageTarget, showSubmit) {
    var success = true;
    var fields  = {};
    var result	= {};
    $("#drupal-elements").remove();
    //Set a container for our form messages.
    $(formTarget).prepend('<div id="drupal-elements"></div>');
    //If we have a valid target, run the validation
    if (formTarget) {
      $("input, select, textarea").each(function() {
        $(this).removeClass('error');
        //Validate null values.
        if (!$(this).val() && $(this).hasClass('required') && $(this).parents().is(formTarget) || !$(this).val() && !$("input[@id="+$(this).attr('id')+"]:checked").val() && $(this).hasClass('required') && $(this).parents().is(formTarget) || !$(this).val() && !$("input[@id="+$(this).attr('id')+"]:checked").val('NULL') && $(this).hasClass('required') && $(this).parents().is(formTarget)) {
          //If the form item value is empty, and required flag it with the error class, and turn success to false. This is a long set of logic.
          success = false;
          $(this).addClass('error');
        } else if ($(this).val() && $(this).parents().is(formTarget) || $("input[@id="+$(this).attr('id')+"]:checked").val() && $(this).parents().is(formTarget)) {
          var fieldVal = '';
          //If the field set is true, return that fields value.
          if ($(this).attr('type') == 'checkbox' || $(this).attr('type') == 'radio') {
            fieldVal = $("input[@id="+$(this).attr('id')+"]:checked").val() ? $("input[@id="+$(this).attr('id')+"]:checked").val() : null;
          } else {
            fieldVal = $("input[@id="+$(this).attr('id')+"]:checked").val() ? $("input[@id="+$(this).attr('id')+"]:checked").val() : $(this).val();
          }
          fields[$(this).attr('id')] = fieldVal;
        }
        //Validate email
        if ($(this).attr("type") == 'text' && $(this).attr("id").match(/email/) && $(this).parents().is(formTarget) && $(this).val()) {
          var pattern = new RegExp(/^(("[\w-\s]+")|([\w-]+(?:\.[\w-]+)*)|("[\w-\s]+")([\w-]+(?:\.[\w-]+)*))(@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$)|(@\[?((25[0-5]\.|2[0-4][0-9]\.|1[0-9]{2}\.|[0-9]{1,2}\.))((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\.){2}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\]?$)/i);
          success = pattern.test($(this).val());
          (success == false) ? $(this).addClass('error') : '';
        } 
      });
      if (success == false) {
        //Set the error message to the drupal-elements we made.
        $("#drupal-elements").prepend('<div class="error">Sorry, but you must fill out all the required form items, please try again.</div>');
      }
      if (showSubmit) {
        //In case the submit hide module is used, show the submit button again.
        $(showSubmit).show();
        $(".hide_submit").remove();
      }
    }
    //Return true/false and the fields that have been processed.
    result.success = success;
    result.fields = fields;
    return result;
	}
  
  /**
   * @name Drupal.PHTools.canFade
   * This allows us to fade objects with classes via drupal settings
   */
  Drupal.PHTools.canFade = function () {
    //Get the fading classes, and apply those fade-outs at load time.
    setTimeout(function(){
      //Set the timeout for our classes.
      $('.status, .warning').fadeOut();
    },Drupal.settings.ph_tools_fade_out_delay);
  }
  
  /**
   * @name Drupal.PHTools.browserDetect
   * Let's us detect a browser via javascript.
   * Browser Detect, this was taken from http://www.quirksmode.org/js/detect.html
   * BrowserDetect.browser
   * BrowserDetect.version
   * BrowserDetect.OS
   */
  Drupal.PHTools.broswerDetect = function () {
    var BrowserDetect = {
      init: function () {
        this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
        this.version = this.searchVersion(navigator.userAgent)
          || this.searchVersion(navigator.appVersion)
          || "an unknown version";
        this.OS = this.searchString(this.dataOS) || "an unknown OS";
      },
      searchString: function (data) {
        for (var i=0;i<data.length;i++)	{
          var dataString = data[i].string;
          var dataProp = data[i].prop;
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
        var index = dataString.indexOf(this.versionSearchString);
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
    var percent;
    if (fade == null || fade == undefined) { fade = false; }
    if (i >= 0 && total >= i) {
      percent = Math.round((parseInt(i) + 1) / parseInt(total) * 100);
      $("#theme-prototype-progress").remove();
      if (percent < 100) {
        $(target).prepend(Drupal.PHTools.theme.progressBar(percent, startMessage));
      } else if (percent >= 100) {
        percent = 100;
        $(target).prepend(Drupal.PHTools.theme.progressBar(percent, endMessage));
        if (fade != false) { //If fade is enabled
          setTimeout(function() { $("#theme-prototype-progress").fadeOut('slow'); } , 2000);
        }
      }
    }
    return false;   
	}
	
	/**
   * @name Drupal.PHTools.theme.progressBar
   * Progress Bar - html wrapper for Drupal.theme('progress');
   */
	Drupal.PHTools.theme.progressBar = function (percent, message) {
    var output;
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