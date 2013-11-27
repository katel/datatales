<?php

/**
 * @file
 * Contains the administrative functions of the Shout Box module.
 *
 * This file is included by the Shout box admin settings form.
 */

/**
 * Admin configure form control on page.
 */
function _drupal_wall_configure_form() {

  $form['drupal_wall'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drupal Wall Settings'),
  );
  $form['drupal_wall']['drupal_wall_wall_post_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of posts to show on the user wall'),
    '#default_value' => (int) variable_get('drupal_wall_wall_post_limit'),
    '#description' => t('Enter range of wall post limit (in numeric). Example "10"'),
    '#size' => 7,
    '#required' => TRUE,
  );

  $form['drupal_wall']['drupal_wall_textbox_type'] = array(
    '#type' => 'radios',
    '#title' => t('Textbox'),
    '#default_value' => variable_get('drupal_wall_textbox_type'),
    '#description' => t('Choose the type of form widget used for entering a wall post.'),
    '#options' => array(
      'textfield' => t('Text Field'),
      'textarea' => t('Text Area'),
      'text_format' => t('Text Format'),
    ),
    '#required' => TRUE,
  );

  // Wall post type.
  $form['drupal_wall']['wall_post'] = array(
    '#markup' => '<strong>' . t('Media type of wall post') . '</strong>',
  );

  $form['drupal_wall']['drupal_wall_post_type_photo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Photos'),
    '#default_value' => variable_get('drupal_wall_post_type_photo'),
  );
  $form['drupal_wall']['drupal_wall_post_type_video'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow share video'),
    '#default_value' => variable_get('drupal_wall_post_type_video'),
  );

  $form['drupal_wall']['button_msg'] = array(
    '#markup' => '<strong>' . t('Enable or Disable operations') . '</strong>',
  );
  $form['drupal_wall']['drupal_wall_older_post_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable "Show older Post" Button'),
    '#default_value' => variable_get('drupal_wall_older_post_button'),
  );
  $form['drupal_wall']['drupal_wall_delete_post_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable "Delete Post" Button'),
    '#default_value' => variable_get('drupal_wall_delete_post_button'),
  );
  $form['drupal_wall']['drupal_wall_edit_post_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable "Edit Post" Button'),
    '#default_value' => variable_get('drupal_wall_edit_post_button'),
  );
  $form['drupal_wall']['drupal_wall_show_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable "Display users comments"'),
    '#default_value' => variable_get('drupal_wall_show_comments'),
  );
  $form['drupal_wall']['drupal_wall_comment_post_textbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable user to "Write a comment".'),
    '#default_value' => variable_get('drupal_wall_comment_post_textbox'),
  );
  $form['drupal_wall']['drupal_wall_likes_post'] = array(
    '#type' => 'checkbox',
    '#title' => t('"Enable "Like or unlike Node & Comment" operation'),
    '#default_value' => variable_get('drupal_wall_likes_post'),
  );

  $form['drupal_wall']['drupal_wall_content_type'] = array(
    '#type' => 'radios',
    '#title' => t('Show content type on user wall post'),
    '#options' => array('Only Drupal wall content', 'All content of user'),
    '#default_value' => variable_get('drupal_wall_content_type', 0),
  );

  $form['drupal_wall']['drupal_wall_likes_node'] = array(
    '#type' => 'textfield',
    '#title' => t('Flag Like machine name'),
    '#default_value' => variable_get('drupal_wall_likes_node'),
    '#description' => t('Please enter the machine name of flag like.&nbsp;') . l(t('Create a new flag'), 'admin/structure/flags/add') . t(', select Flag type = "Nodes" and Bundles/content type = "Drupal Wall".'),
    '#states' => array(
      'visible' => array(
        ':input[name="drupal_wall_likes_post"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['drupal_wall']['drupal_wall_likes_comment'] = array(
    '#type' => 'textfield',
    '#title' => t("Flag's Comment Like machine name"),
    '#default_value' => variable_get('drupal_wall_likes_comment'),
    '#description' => t('Please enter the machine name of flag comment like.&nbsp;') . l(t('Create a new flag'), 'admin/structure/flags/add') . t(', select Flag type = "Comments" and Bundles/content type = "Drupal Wall comment".'),
    '#states' => array(
      'visible' => array(
        ':input[name="drupal_wall_likes_post"]' => array('checked' => TRUE),
      ),
    ),
  );
  return system_settings_form($form);
}

/**
 * Validate admin configure form control on page.
 */
function _drupal_wall_configure_form_validate($form, $form_state) {
  if (isset($form_state['values']['drupal_wall_wall_post_limit'])) {
    if (!(is_numeric($form_state['values']['drupal_wall_wall_post_limit']))) {
      form_set_error('drupal_wall_wall_post_limit', t('Wall post limit should be numeric. Eg: 10'));
    }
    elseif ($form_state['values']['drupal_wall_wall_post_limit'] < 1 || $form_state['values']['drupal_wall_wall_post_limit'] > 999) {
      form_set_error('drupal_wall_wall_post_limit', t('Wall post limit should be between 1 to 999.'));
    }
  }
}
