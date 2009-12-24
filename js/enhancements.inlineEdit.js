/**
 * Jquery Inline Node Edit
 * This adds inline editing to nodes creates, implicitely intercepts node editing
 * And allows for inline editing without the use of lightboxes, or going to another page
 * Features:
 * Edit
 * Add Another
 * Delete
 * Preview (Without that annoying CSS)
 *
 * TODO: Convert to WYSIWYG Module - make sure to add that to this module's dependencies
 */
if (Drupal.jsEnabled) {
	
	$(document).ready(function() {
      //admin links ajax forms
      $('.node-admin-links li.ajax-node-add a').bind('click', function(){
          var type   	= $(this).attr('ref');
          var nid    	= $(this).attr('id');
          var target    = '#node-form';
          var fckIndex  = ( $('#page-content-main a.ajax-edit').index( $(this) ) +1 );
          var fckId     = 'oFCK_'+fckIndex;
          var originTxt = 'edit-body';
          //Drupal.trace($(this).attr('ref'));
          $.getJSON(Drupal.settings.ajax_node_edit,
              {type : type, nid : nid},
              function (data) {
                
                //put the form below the node
                $('#node-'+nid).before(Drupal.urlDecode(data.form));
                //Hide all admin form links
                $('a.ajax-edit').hide();
                
                //add form handlers after everything loads
                addHandlers(type,target,nid,fckId,originTxt,fckIndex);
                
              }
          );
          return false;
      });
      //end document ready
	});
    
    //add handlers for submit, preview and delete
	function addHandlers(type,target,nid,fckId,originTxt,fckIndex) {
      
	  var submitHandler = function() {
        
        var validate = Drupal.validateForm( target, false, '#'+$(this).attr('id') );
        Drupal.scrollTo(target);
        Drupal.trace(validate);
        //Drupal.trace($(this).attr('id'));
        
        switch (true) {
          case $(this).attr('id') == 'edit-add-another':
            if ( validate.success == true ) {
              addAnotherSubmit(type,target,nid,fckId,originTxt,fckIndex,validate);
            }
          break;
          case $(this).attr('id') == 'edit-submit':
            if ( validate.success == true ) {
              saveSubmit(type,target,nid,fckId,originTxt,fckIndex,validate);
            }
          break;
          case $(this).attr('id') == 'edit-preview':
            if ( validate.success == true ) {
              previewSubmit(type,target,nid,fckId,originTxt,fckIndex,validate);
            }
          break;
          case $(this).attr('id') == 'edit-delete':
            //Drupal.trace('delete');
            var exec = confirm("Delete this "+type+"? \nThis action cannot be undone.");
            if ( exec ) {
              deleteSubmit(type,target,nid,fckId,originTxt,fckIndex,validate);
            }
          break;
          case $(this).attr('id') == 'edit-cancel':
            //Drupal.trace('cancel');
            cancelSubmit(type,target,nid,fckId,originTxt,fckIndex);
          break;
          default:
            Drupal.trace('Error, not a valid callback.');
          break;
        }
        
        return false;
      }
      
      $("form"+target).submit(function () { submitHandler(); return false; });
      $("form"+target+" input[type=submit]").click(submitHandler);
      $('form'+target).attr('action',Drupal.settings.ajax_form_action);
      $("form"+target+" textarea[id^=oFCK_]").attr('id',fckId);
      $("form"+target+" a[id^=switch_]").attr('id','switch_'+fckId);
      $("form"+target+" div[id^=fck_]").attr('id','fck_'+fckId);
      Drupal.attachBehaviors($('form'+target));
      //Toggle the editor to on
      Toggle(fckId, originTxt, '', '', 1);
      $('#edit-body-wrapper > div.resizable-textarea').hide(); //86 teaser

	}
    
    //generic save function, processes cck fields
    function saveSubmit(type,target,nid,fckId,originTxt,fckIndex) {
      
      Drupal.theme( 'progress', target, 100, 50, 'Saving...', 'Save Successful!' );
      $.post(Drupal.settings.ajax_node_add, Drupal.serialize({ node: validate.fields }), function (data) {
        
        data = eval('('+data+')');
        Drupal.trace(data);
        Drupal.theme( 'progress', target, 100, 100, 'Saving...', 'Save Successful!' );
        
      });
      
    }
    
    //runs preview from drupal
    function previewSubmit(type,target,nid,fckId,originTxt,fckIndex) {
      return;
    }
    
    //redirects you directly to that nodes delete confirmation
    function deleteSubmit(type,target,nid,fckId,originTxt,fckIndex) {
      
      return;
    }
    
    //takes you to add another
    function addAnotherSubmit(type,target,nid,fckId,originTxt,fckIndex) {
      return;
    }
    
    function cancelSubmit(type,target,nid,fckId,originTxt,fckIndex) {
      
      //remove form
      $("form"+target).remove();
      //turn off the editor
      //Drupal.trace(fckIsRunning[fckId]);
      delete fckIsRunning[fckId];
      delete fckIsLaunching[fckId];
      delete fckLaunchedTextareaId[fckId];
      delete fckLaunchedJsId[fckId];
      delete fckFirstrun[fckId];
      //show admin links on cancel
      $('a.ajax-edit').show();
      
    }
	
}