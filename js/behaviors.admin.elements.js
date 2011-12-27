/**
 * Module jQuery functions and Miscellaneous functions specific to this module.
 */

//If javascript is enabled
if (Drupal.jsEnabled) {
  //Variables
  var data, rows, row, target, name, item, items, output, copy, next, axis, handle;
  var attributes = items = {};
  //Set module namespace.
  Drupal.phToolsAdmin       = Drupal.phToolsAdmin || {};
  Drupal.phToolsAdmin.theme = Drupal.phToolsAdmin.theme || {};
  //Bind page events.
  Drupal.behaviors.phToolsAdminInit = function(context) {
    //Delete form field for meta tags.
    if($('button.delete-me').length){
      $('button.delete-me', context).each(function() {
        $(this).bind('click', Drupal.phToolsAdmin._del);
      });
    }
    //Add Form Field
    if($('button.add-me').length){
      $('button.add-me', context).each(function() {
        $(this).bind('click', Drupal.phToolsAdmin._add);
      });
    }
    //Toggle all module checkboxes on or off.
    if($('a.settings-toggle').length) {
      $('a.settings-toggle', context).each(function() {
        $(this).bind('click', Drupal.phToolsAdmin._toggle);
      });
    }
    //Make library fieldsets draggable.
    if ($('ul#phtools-sortable-library').length) {
      $('ul#phtools-sortable-library', context).each(function(){
        //Set sortable attributes.
        //attributes.containment = 'parent';
        attributes.items       = 'li';
        attributes.opacity     = '.6';
        attributes.axis        = 'y';
        attributes.placeholder = 'ui-state-highlight';
        attributes.delay       = 300;
        attributes.distance    = 8;
        //Weight is not a UI attribute.
        attributes.weights     = {};
        Drupal.phToolsAdmin._attachSortable($(this), attributes);
        Drupal.phToolsAdmin._setSortableWeight($(this));
      });
    }
  }
  //Set weight for an element.
  Drupal.phToolsAdmin._setSortableWeight = function(target) {
    //Update the weight form field.
    target.bind('sortstop', function(event, ui) {
     items = $(event.target).children('li[id^=phtools-sortable]');
     items.each(function(e) {
       item = $(this).find('.form-weight');
       item.val(e);
     });
    });
  }
  //Attach sortable to an element.
  Drupal.phToolsAdmin._attachSortable = function(target, attributes) {
    //Add sortable.
    target.sortable(attributes);
  }
  //Delete a row.
  Drupal.phToolsAdmin._del = function() {
    //Get the Rows already built.
    items  = Drupal.phToolsAdmin._find($(this));
    //Get the Rows already built.
    rows   = items.rows;
    //Find our target.
    target = items.target;
    //Find our target.
    if (rows.length > 1) {
      $(this).parents('.form-item').fadeOut(function() {
        $(this).remove();
        Drupal.theme('message', target, 'Row Deleted.', 'status');
      });
    } else {
      Drupal.theme('message', target, 'You must have at least one row, you cannot delete any more rows.', 'warning');
    }
  }
  //Add a row.
  Drupal.phToolsAdmin._add = function() {
    //Get the Rows already built.
    items  = Drupal.phToolsAdmin._find($(this));
    //Get the Rows already built.
    rows   = items.rows;
    //Get the row we are cloning.
    row    = rows[0];
    //Reset this target's messages.
    $(this).closest('div[class^=fieldset-]').find('.message').each(function() {$(this).remove();});
    //Generate the next number for a row.
    next = rows[rows.length-1].id;
    next = next.replace(/[a-zA-z\-]/ig, '');
    next = (parseInt(next) + 1);
    //Clone a unique form item.
    copy = $('#'+row.id).clone();
    copy.attr('id', copy.attr('id').replace(/[0-9]/ig, next));
    copy.find('.form-item, input').each(function() {
      //Replace out id's and name with our highest row value.
      $(this).attr('id', $(this).attr('id').replace(/[0-9]/ig, next));
    });
    copy.find('input').each(function() {
      //Replace out id's and name with our highest row value.
      $(this).attr('name', $(this).attr('name').replace(/[0-9]/ig, next));
      $(this).val('');
    });
    //Find our target.
    target = items.target;
    //Append our cloned form item.
    copy.appendTo(target);
    //Set a status message.
    Drupal.theme('message', target, 'New Row Added.', 'status');
    //Re-attach Drupal behaviors.
    Drupal.attachBehaviors(copy);
  }
  //Toggle this element and it's children.
  Drupal.phToolsAdmin._toggle = function () {
    //Check to see if we are selected or not, if we are, select all. If not deselect all but what was selected.
    var checked = $(this).closest('div[class^=fieldset-]').find('.form-checkbox:checked');
    var rows    = $(this).closest('div[class^=fieldset-]').find('.form-checkbox');
    if ($(this).hasClass('all-on')) {
      $(this).removeClass('all-on').text(Drupal.t('Toggle All Off'));
      rows.each(function() {
        $(this).attr('checked', true);
      });
    } else {
      $(this).addClass('all-on').text(Drupal.t('Toggle All On'));
      rows.each(function() {
        $(this).attr('checked', false);
      });
    }
    return false;
  }
  //Find a particular set of rows if they exist.
  Drupal.phToolsAdmin._find = function (manager) {
    //Clear out items objec,t so there is no cross contamination.
    items = {};
    //Get the rows we need from this item.
    items.rows   = manager.closest('div[class^=fieldset-]').children('.form-item');
    //Get the target for this operation.
    items.target = manager.closest('div[class^=fieldset-]');
    //Return our items.
    return items;
  }

  /**
   * Theme implenentations.
   */
  //Theme prototype message.
  Drupal.theme.prototype.message = function(target, text, status) {
    //Remove any messages set.
    target.find('.message').each(function() {$(this).remove();});
    //Set our new Message.
    target.append(Drupal.phToolsAdmin.theme.setMessage(Drupal.t(text), status));
    //Set our messages to fade out eventually.
    setTimeout(function() {
      //Set the timeout for our classes.
      $('.status, .warning').fadeOut();
    },'1500');
  }
  //Set a Drupal Message.
  Drupal.phToolsAdmin.theme.setMessage = function(text, status) {
    output = '';
    output += '<div class="message '+status+'"><p><h4>';
    output += text;
    output += '</h4></p></div>';
    return output;
  }
}