<?php

/**
 * @file
 * Contains the logical functions of the Drupal wall module.
 */

/**
 * Drupal wall form for post input textarea and post button.
 */
function _drupal_wall_content_post_form() {

  global $base_url;

  $form['drupal_wall_status'] = array(
    '#type' => variable_get('drupal_wall_textbox_type'),
    '#format' => 'filtered_html',
    '#required' => FALSE,
    '#attributes' => array(
      'placeholder' => t("What's on your mind?"),
    ),
    '#prefix' => '<div class="drupal_wall_post_status">',
  );

  $media_share['none'] = '-none-';
  if (variable_get('drupal_wall_post_type_photo') == 1) {
    $media_share['photo'] = 'Add Photo';
  }
  if (variable_get('drupal_wall_post_type_video') == 1) {
    $media_share['video'] = 'Share Video';
  }

  if (variable_get('drupal_wall_post_type_photo') == 1 || variable_get('drupal_wall_post_type_video') == 1) {
    $form['drupal_wall_photo_video_enable'] = array(
      '#type' => 'select',
      '#title' => t('Share'),
      '#options' => $media_share,
      '#states' => array(
        'visible' => array(
          ':input[name="drupal_wall_group_redirect"]' => array('value' => '-1'),
        ),
      ),
    );
  }

  // Hiding image option if type photo is disable.
  if (variable_get('drupal_wall_post_type_photo') == 1) {
    $form['drupal_wall_photo_status'] = array(
      '#type' => 'managed_file',
      '#upload_location' => 'public://drupal_wall//',
      '#upload_validators' => array(
        'file_validate_extensions' => array('gif png jpg jpeg'),
      ),
      '#states' => array(
        'visible' => array(
          ':input[name="drupal_wall_photo_video_enable"]' => array('value' => 'photo'),
        ),
      ),
    );
  }

  // Hiding video option if type video is disable.
  if (variable_get('drupal_wall_post_type_video') == 1) {
    $form['drupal_wall_video_status'] = array(
      '#type' => 'textfield',
      '#description' => t('Please enter the youtube video URL. Example:') . ' http://www.youtube.com/watch?v=vw-G-adwRNU',
      '#states' => array(
        'visible' => array(
          ':input[name="drupal_wall_photo_video_enable"]' => array('value' => 'video'),
        ),
      ),
    );
  }
  $form['drupal_wall_status_post'] = array(
    '#value' => t('Post'),
    '#type' => 'submit',
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array('post_btn'),
    ),
  );
  return $form;
}

/**
 * Validate form for Drupal wall post box.
 */
function _drupal_wall_content_post_form_validate($form, $form_state) {

  if (isset($form_state['values']['drupal_wall_photo_video_enable']) && $form_state['values']['drupal_wall_photo_video_enable'] == 'video') {
    if (isset($form_state['values']['drupal_wall_video_status']) && $form_state['values']['drupal_wall_video_status'] != '') {
      $video_url = $form_state['values']['drupal_wall_video_status'];
      $pos_v = strrpos($video_url, 'v=', -1);
      if ((!$pos_v)) {
        form_set_error('drupal_wall_video_status', t('Sorry ! Not able to recognize the Youtube video.'));
      }
      if (stristr($video_url, 'http://www.youtube.com/') === FALSE) {
        form_set_error('drupal_wall_video_status', t('Sorry ! Only Youtube video are allowed to shated.'));
      }
    }
  }
}

/**
 * Submit form to save the post status content (save node).
 */
function _drupal_wall_content_post_form_submit($form, &$form_state) {

  if (variable_get('drupal_wall_textbox_type') == 'text_format') {
    $status_msg = $form_state['values']['drupal_wall_status']['value'];
  }
  else {
    $status_msg = $form_state['values']['drupal_wall_status'];
  }
  $video_exist = FALSE;
  $photo_exist = FALSE;
  $msg = NULL;

  if (isset($form_state['values']['drupal_wall_photo_video_enable'])) {
    if ($form_state['values']['drupal_wall_photo_video_enable'] == 'video' && isset($form_state['values']['drupal_wall_video_status']) && $form_state['values']['drupal_wall_video_status'] != '') {
      $video_status = $form_state['values']['drupal_wall_video_status'];
      $video_exist = TRUE;
      $msg = 'Shared a new video';
    }
    elseif ($form_state['values']['drupal_wall_photo_video_enable'] == 'photo' && isset($form_state['values']['drupal_wall_photo_status']) && $form_state['values']['drupal_wall_photo_status'] != '') {
      $photo_status = $form_state['values']['drupal_wall_photo_status'];
      $photo_exist = TRUE;
      $msg = 'Added a new photo';
    }
  }

  if ($status_msg != '' || $photo_exist || $video_exist) {

    if ($form_state['values']['form_id'] == '_drupal_wall_content_post_form') {
      global $user;
      $new_node = new stdClass();

      $new_node->type = 'drupal_wall';
      node_object_prepare($new_node);
      $new_node->uid = $user->uid;
      $new_node->name = $user->name;

      $new_node->language = 'und';
      $new_node->body[$new_node->language][0]['value'] = $status_msg;
      $new_node->body[$new_node->language][0]['format'] = 'full_html';

      // Taking first 50 char of status_msg as title.
      $status_msg_title = drupal_substr(strip_tags($status_msg), 0, 49);
      $pos = strrpos($status_msg_title, ' ', -1);
      if ($pos != NULL) {
        $status_msg_title = drupal_substr($status_msg_title, 0, $pos) . '...';
      }
      elseif ($status_msg == '') {
        $status_msg_title = $msg;
      }
      else {
        $status_msg_title .= '...';
      }
      // Saving image to photo field in node.
      if ($photo_exist) {
        $image_id = $photo_status;
        if (isset($image_id) && $image_id != NULL) {
          // Load the file via file.fid.
          $file = file_load($image_id);
          if (is_object($file)) {
            $file->status = FILE_STATUS_PERMANENT;
            file_save($file);
            file_usage_add($file, 'drupal_wall', 'image', $image_id);

            $image_path = file_create_url($file->uri);
            $image = (object) array(
              'uid' => $user->uid,
              'uri' => $file->uri,
              'filemime' => file_get_mimetype($image_path),
              'status' => 1,
            );
            if ($image = file_move($image, 'public://drupal_wall//')) {
              $new_node->field_drupal_wall_photos[$new_node->language][0] = (array) $image;
            }
            else {
              form_set_error('drupal_wall_photo_status', t('Failed to write the uploaded file in "sites/default/" file folder. Please provide write permission to this directory'));
            }
          }
        }
      }
      // Save video URL to video field of node.
      if ($video_exist) {
        $video_url = $video_status;
        $pos_v = strrpos($video_url, 'v=', -1);
        if ($pos_v) {
          $video_url = 'http://www.youtube.com/embed/' . drupal_substr($video_url, $pos_v + 2);
          $new_node->field_drupal_wall_videos[$new_node->language][0]['value'] = $video_url;
          $new_node->field_drupal_wall_videos[$new_node->language][0]['format'] = 'plain_text';
        }
      }

      $new_node->language = 'und';
      $new_node->title = $status_msg_title;

      $new_node->comment = 2;
      $new_node->status = 1;
      $new_node->promote = 1;
      $new_node->revision = 0;

      $new_node->changed = $_SERVER['REQUEST_TIME'];
      $new_node->created = $_SERVER['REQUEST_TIME'];
      node_submit($new_node);
      node_save($new_node);

      drupal_set_message(t('Your post has been saved !'));
    }
  }
  else {
    drupal_set_message(t('Sorry ! Nothing has shared to post.'), 'warning');
  }
}

/**
 * Drupal wall Comment form for user comment posts.
 */
function _drupal_wall_comment_post_form($form_state, $args) {

  global $base_url;
  // Comment display form.
  $form['drupal_wall_comment'] = array(
    '#type' => 'textarea',
    '#resizable' => FALSE,
    '#attributes' => array(
      'placeholder' => ' ' . t('write a comment...'),
    ),
    '#required' => TRUE,
    '#prefix' => '<div style="margin-bottom:-8px;">',
    '#suffix' => '</div>',
  );
  $form['drupal_wall_comment_nid'] = array(
    '#type' => 'hidden',
    '#value' => $args['build_info']['args']['0'],
  );

  $form['drupal_wall_comment_uid'] = array(
    '#type' => 'hidden',
    '#value' => $args['build_info']['args']['1'],
  );

  $form['drupal_wall_submit'] = array(
    '#type' => 'button',
    '#value' => t('Post'),
    '#ajax' => array(
      'callback' => '_drupal_wall_comment_ajax_form_post',
      'wrapper' => 'div_append_next_user_comment_' . $args['build_info']['args']['0'],
      'method' => 'append',
      'effect' => 'none',
    ),
    '#prefix' => '<div style="margin-bottom:-10px;">',
    '#suffix' => '</div>',
  );

  $form['#action'] = url('user/' . variable_get('drupal_wall_user_id'));
  $form['#submit'] = array('_drupal_wall_comment_ajax_form_post');
  return $form;
}

/**
 * Ajax call to save users comments for its respective node.
 */
function _drupal_wall_comment_ajax_form_post($form, $form_state) {

  global $base_url;
  global $user;
  $comment_value = trim(strip_tags($form_state['values']['drupal_wall_comment']));

  if ($comment_value != '') {

    // Taking first 50 char of status_msg as title.
    $comment_msg = drupal_substr($comment_value, 0, 49);
    $pos = strrpos($comment_msg, ' ', -1);

    // Append string '...'
    if ($pos != NULL) {
      $comment_msg = drupal_substr($comment_msg, 0, $pos) . '...';
    }
    else {
      $comment_msg .= '...';
    }
    $comment = new stdClass();

    $comment->nid = $form_state['input']['drupal_wall_comment_nid'];
    $comment->cid = 0;
    $comment->pid = 0;
    $comment->uid = $user->uid;
    $comment->mail = $user->mail;

    $comment->created = time();
    $comment->is_anonymous = 0;
    $comment->homepage = '';
    $comment->status = COMMENT_PUBLISHED;
    $comment->language = LANGUAGE_NONE;

    $comment->subject = $comment_msg;
    $comment->comment_body[$comment->language][0]['value'] = $comment_value;
    $comment->comment_body[$comment->language][0]['format'] = 'filtered_html';

    // Saving a comment.
    $new_comment = comment_submit($comment);
    $cid = comment_save($comment);

    if (is_numeric($new_comment->cid)) {
      // Returning comment result in its respective comment block.
      $html = '<div class="comment" id="drupal_wall_comment_cid_' . $new_comment->cid . '"><div class="comment_left"><a href="' . $base_url . '/user/' . $user->uid . '"><img src="' . _drupal_wall_user_profile_picture($user->uid) . '" width="32px"></a></div><div class="comment_right"><div class="comment_delete">' . drupal_render(drupal_get_form('_drupal_wall_delete_comment_form', $new_comment->cid, $user->uid)) . '</div><strong><a href="' . $base_url . '/user/' . $user->uid . '">' . ucwords($user->name) . '</a>&nbsp;</strong>' . $comment_value . '<div class="caption">' . date('F j, Y', time()) . ' at ' . date('h:ia', time()) . '</div></div></div>';

      $form_state['values']['drupal_wall_comment'] = '';
      return $html;
    }
    else {
      return '<span style="color:red">No able to save comment</span>';
    }
  }
}

/**
 * Drupal wall Delete form for user to delete post.
 */
function _drupal_wall_delete_edit_node_form($form_state, $args) {
  $form = array();
  global $base_url;
  global $user;
  $drupal_wall_path = drupal_get_path('module', 'drupal_wall');

  if (variable_get('drupal_wall_edit_post_button') == 1) {
    $img = '<img src="' . $base_url . '/' . $drupal_wall_path . '/images/edit-icon.png' . '" >';
    $redirect = $base_url . '/node/' . $args['build_info']['args']['0'] . '/edit?destination=user/' . $user->uid;
    $form['drupal_wall_edit_post'] = array(
      '#markup' => l($img, $redirect, array('attributes' => array('class' => 'anchor-class'), 'html' => TRUE)),
    );
  }
  if (variable_get('drupal_wall_delete_post_button') == 1) {
    $form['drupal_wall_delete_post'] = array(
      '#value' => t('Delete Button'),
      '#type' => 'image_button',
      '#src' => $base_url . '/' . $drupal_wall_path . '/images/delete-icon.png',
      '#attributes' => array(
        'class' => array('delete_btn'),
      ),
      '#ajax' => array(
        'callback' => '_drupal_wall_delete_node_post',
        'wrapper' => 'drupal_wall_post_nid_' . $args['build_info']['args']['0'],
        'method' => 'replace',
        'event' => 'click',
      ),
    );
  }

  if (variable_get('drupal_wall_delete_post_button') == 1 || variable_get('drupal_wall_edit_post_button') == 1) {
    $form['drupal_wall_post_nid'] = array(
      '#type' => 'hidden',
      '#value' => $args['build_info']['args']['0'],
    );

    $form['drupal_wall_post_uid'] = array(
      '#type' => 'hidden',
      '#value' => $args['build_info']['args']['1'],
    );
    $form['#action'] = url('user/' . variable_get('drupal_wall_user_id'));
  }
  return $form;
}

/**
 * Delete form for user to delete its post.
 */
function _drupal_wall_delete_node_post($form, $form_state) {

  $nid = $form_state['input']['drupal_wall_post_nid'];
  // Deleting all content for node 'nid'.
  node_delete($nid);
  // Replace with null (hiding).
  return '';
}

/**
 * Drupal wall delete form for user to delete post.
 *
 * @param array $form
 *   Drupal submit form array.
 *
 * @param string $args
 *   Arguments send to function.
 *
 * @return array
 *   Drupal submited form content array.
 */
function _drupal_wall_delete_comment_form($form, $args) {
  global $base_url;
  global $user;
  $drupal_wall_path = drupal_get_path('module', 'drupal_wall');
  $form = array();

  // Delete buuton visible only to user who post that comment.
  if ($args['build_info']['args']['1'] == $user->uid && variable_get('drupal_wall_delete_post_button') == 1) {
    $form['drupal_wall_delete_comment_' . $args['build_info']['args']['0']] = array(
      '#value' => t('Delete Comment'),
      '#type' => 'image_button',
      '#src' => $base_url . '/' . $drupal_wall_path . '/images/delete-icon.png',
      '#action' => url('user/' . variable_get('drupal_wall_user_id')),
      '#ajax' => array(
        'callback' => '_drupal_wall_delete_comment_form_submit',
        'wrapper' => 'drupal_wall_comment_cid_' . $args['build_info']['args']['0'],
        'method' => 'replace',
        'event' => 'click',
      ),
      '#attributes' => array(
        'class' => array('delete_btn'),
      ),
    );

    $form['drupal_wall_comment_cid'] = array(
      '#type' => 'hidden',
      '#value' => $args['build_info']['args']['0'],
    );

    $form['drupal_wall_comment_uid'] = array(
      '#type' => 'hidden',
      '#value' => $args['build_info']['args']['1'],
    );
    $form['#action'] = url('user/' . variable_get('drupal_wall_user_id'));
  }
  return $form;
}

/**
 * Delete form for user to delete its Comment.
 *
 * @param array $form
 *   Drupal submit form array.
 *
 * @param string $form_state
 *   Drupal form state array.
 *
 * @return array
 *   Delete comment node and return nothing.
 */
function _drupal_wall_delete_comment_form_submit($form, $form_state) {
  $cid = $form_state['input']['drupal_wall_comment_cid'];

  // Deleting all content for node Comment via cid.
  comment_delete($cid);
  // Replace with null (hiding).
  return '';
}
