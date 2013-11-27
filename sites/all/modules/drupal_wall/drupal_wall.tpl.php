<?php
/**
 * @file
 * Default theming to display a Drupal wall status posts in a drupal
 * block.
 *
 * Available variables:
 *
 * Drupal wall - wall post (object array )
 * - $wall_post : Consisit aray of drupal wall content type wall post of the
 *   desire user.
 *   It contains the Node: nid, uid, created, title, body, comment.
 *   
 * Drupal user information (object array) 
 * - $user_info : Consisit of aray of drupal user basic information.
 *   User info => users ID (uid), name, mail & picture.
 *
 * @see _drupal_wall_user_profile_picture($user_id)
 */

  global $base_url;
  global $user;
  $path = drupal_get_path('module', 'drupal_wall');

  // To Check complete array of wall post & user info.
/*
  echo '<pre>';
  print_r($wall_post);
//  print_r($user_info);
  echo '</pre>';
*/
?>

<!-- Drupal wall block : starts here ! -->
<div id="drupal_wall_append_older_wall_post">
  <?php
    for($i = 0; $i < count($wall_post); $i++) :
  ?>

  <div class='drupal_wall' id="<?php print 'drupal_wall_post_nid_' . $wall_post[$i]->nid; ?>">
    <!-- Edit - delete button form button: starts here ! -->
    <?php
      if ($user->uid == $user_info['uid']) :
    ?>
    <div class="edit_delete">
      <?php print drupal_render(drupal_get_form('_drupal_wall_delete_edit_node_form', $wall_post[$i]->nid, $user->uid)); ?> 
    </div>
    <?php endif; ?>    
    <!-- Edit - delete button: ends here ! -->
      
    <!-- Left Image Icon : starts here ! -->
    <div class='wallContent_left'>
    <?php 
      if (isset($user_info['picture'])) :
        print '<a href="' . $base_url . '/user/' . $user_info['uid'] . '" ><img src="' . $user_info['picture'] . '" width="50px" /></a>';
      endif;
    ?>
    </div>
    <!-- Left Image Icon : ends here ! -->

    <!-- Wall right content Block : starts here ! -->
    <div class="wallContent_right">
      <!-- Headline Block : Starts here ! -->
      <strong>
        <?php
          if (isset($user_info['name'])) :
            print '<a href="' . $base_url . '/user/' . $user_info['uid'] . '" >' . ucwords($user_info['name']) . '</a>';
          endif;
        ?>
      </strong>
      <span class="headline">
        <?php 
        if ($wall_post[$i]->type != 'drupal_wall') :
          print 'created a <a href="' . $base_url . '/node/' . $wall_post[$i]->nid . '" >new ' . ucfirst($wall_post[$i]->type) . '</a>';
        else :
        if (isset($wall_post[$i]->field_drupal_wall_videos_value) && $wall_post[$i]->field_drupal_wall_videos_value != '') :
            print 'shared a <a href="' . $base_url . '/node/' . $wall_post[$i]->nid . '" >new video</a>';
          elseif (isset($wall_post[$i]->field_drupal_wall_photos_fid) && $wall_post[$i]->field_drupal_wall_photos_fid != '') :
            print 'added a <a href="' . $base_url . '/node/' . $wall_post[$i]->nid . '" >new picture</a>';
          else :
            print 'updates its <a href="' . $base_url . '/node/' . $wall_post[$i]->nid . '" >status</a>';
          endif;
        endif;

        ?>
        <div class="caption">
          <?php
            if (isset($wall_post[$i]->created)) :
              print date('F j, Y', $wall_post[$i]->created);
              print ' at ' . date('h:ia', $wall_post[$i]->created);
            endif;
          ?>
        </div>
        </span>
        <!-- Headline block : ends here ! -->

        <!-- User Content block : starts here ! -->
        <div class="userContent"> 
          <?php 
            // What's on my mind.
            if (isset($wall_post[$i]->body_value)) :
              print $wall_post[$i]->body_value;
            endif;
          ?>
        </div>
        <!-- User Content block : ends here ! -->

        <!-- User Photo block : starts here ! -->
        <?php
        if (isset($wall_post[$i]->field_drupal_wall_photos_fid) && $wall_post[$i]->field_drupal_wall_photos_fid != '') :
        ?>
        <div class="photo_status">
          <?php
            $img_path = _drupal_wall_status_picture($wall_post[$i]->field_drupal_wall_photos_fid);
            print '<a href="' . $img_path . '"><img src="' . $img_path . '" ></a>';
        ?>
        </div>
        <?php endif; ?>
        <!-- User Photo block : ends here ! -->

		<!-- Youtube video block : starts here ! -->
        <?php
        if (isset($wall_post[$i]->field_drupal_wall_videos_value) && $wall_post[$i]->field_drupal_wall_videos_value != '') :
        ?>
        <div class="video_status">
          <?php
            print '<iframe  scrolling="no" width="445" height="250" src="' . $wall_post[$i]->field_drupal_wall_videos_value . '" frameborder="0" allowfullscreen>Your browser does not support iframes.</iframe>';
          ?>
        </div>
        <?php endif; ?>
        <!-- Youtube video : ends here ! -->

        <!-- Likes block : starts here ! -->
          <?php
            $flag_name = variable_get('drupal_wall_likes_node');
            $flag_type = flag_get_flag($flag_name);
            if ($user->uid != 0 && $wall_post[$i]->type == 'drupal_wall') :
            if(module_exists('flag') && variable_get('drupal_wall_likes_post') == 1 && $flag_type != NULL) :  ?>
          <div class="likes">
            <?php
              if ($flag_type != NULL && in_array('drupal_wall', $flag_type->types)) :
                print flag_create_link($flag_name, $wall_post[$i]->nid);
              endif;

              $count_flag = flag_get_counts('node', $wall_post[$i]->nid);
              if (isset($count_flag[$flag_name]) && $count_flag[$flag_name] > 0) :
                print $count_flag[$flag_name] . ' people like this.';
              endif;
            ?>
            </div>
            <?php endif;
            endif;
             ?>
        <!-- Likes block : ends here ! -->

        <!-- Comment Block : Starts here ! -->
        <?php
          $count_comments = 0;
          if ($wall_post[$i]->comment != 0 && isset($wall_post[$i]->comments)) :
            $count_comments = count($wall_post[$i]->comments);
            if ($count_comments > 0) :
        ?>
        <div class="commentView">
          <?php print '<a href="' . $base_url . '/node/' . $wall_post[$i]->nid . '#comments" >View all ' . count($wall_post[$i]->comments) . ' comments</a>'; ?>
        </div>
          <?php endif;
            endif;
              // End outer if.
          ?>

        	<?php 
            if ($count_comments > 0) :
              for ($key = 0; $key < $count_comments && $key < 7; $key++) :
          ?>
          <div class="comment" id="<?php print 'drupal_wall_comment_cid_' . $wall_post[$i]->comments[$key]->cid; ?>">
            <div class="comment_left">
              <?php
                $friend_id = $wall_post[$i]->comments[$key]->uid;
                if (isset($friend_id) && _drupal_wall_user_profile_picture($friend_id) != FALSE) :
                  print '<a href="' . $base_url . '/user/' . $friend_id . '" ><img src="' . _drupal_wall_user_profile_picture($friend_id) . '" width="32px"></a>';
                endif;
              ?>
            </div>
            <div class="comment_right">
				
            <!-- Delete Comment Block : Starts here ! -->
            <div class="comment_delete">
              <?php print drupal_render(drupal_get_form('_drupal_wall_delete_comment_form', $wall_post[$i]->comments[$key]->cid, $wall_post[$i]->comments[$key]->uid)); ?>
            </div>
            <!-- Delete Comment Block : ends here ! -->
			
              <strong>
              <?php
                if (isset($wall_post[$i]->comments[$key]->name)) :
                  print '<a href="' . $base_url . '/user/' . $wall_post[$i]->comments[$key]->uid . '">' . ucwords($wall_post[$i]->comments[$key]->name) . '</a> ';
                endif;
              ?>
              </strong>
                <?php
                  if (isset($wall_post[$i]->comments[$key]->comment_body_value)) :
                    print strip_tags($wall_post[$i]->comments[$key]->comment_body_value);
                 endif;
                ?>
              <div class="caption">
                <?php
                  if (isset($wall_post[$i]->comments[$key]->created)) :
                    print date('F j, Y', $wall_post[$i]->comments[$key]->created);
                    print ' at ' . date('h:ia', $wall_post[$i]->comments[$key]->created);
                  endif;

                  // Liking option for Comments.
                  if(module_exists('flag') && variable_get('drupal_wall_likes_post') == 1) :

                    $comment_flag_name = variable_get('drupal_wall_likes_comment');
                    $commment_count_flag = flag_get_counts('comment', $wall_post[$i]->comments[$key]->cid);
                    if (isset($commment_count_flag[$comment_flag_name]) && $commment_count_flag[$comment_flag_name] > 0) :
                      print '<span class="like_comment">' . $commment_count_flag[$comment_flag_name] . '&nbsp;';
                    endif;

                    $commment_flag_type = flag_get_flag($comment_flag_name);
                    if ($commment_flag_type != NULL && in_array('comment_node_drupal_wall', $commment_flag_type->types)) :
                      print flag_create_link($comment_flag_name, $wall_post[$i]->comments[$key]->cid) . '</span>';
                    endif;
                  endif;
                ?>
              </div>
            </div>
          </div>
            <?php
                endfor;
            endif;
              // End outer if.
            ?>
            <!-- Comment Block : Ends here ! -->
            
            <!-- For AJAX comment post -->
            <div id="div_append_next_user_comment_<?php echo $wall_post[$i]->nid; ?>">
            </div>
            <!-- For AJAX comment post : Ends here-->

          <!-- write a comment Block : starts here ! -->
          <?php 
            if ($user->uid != 0 && $wall_post[$i]->comment == 2 && variable_get('drupal_wall_comment_post_textbox') == 1) :
          ?>
          <div class="comment">
            <div class="comment_left">
              <?php
                if (_drupal_wall_user_profile_picture($user->uid) != FALSE) :
                  print '<img src="' . _drupal_wall_user_profile_picture($user->uid) . '" width="32px">';
                endif;
              ?>
            </div>
            <div class="comment_right">
              <?php print drupal_render(drupal_get_form('_drupal_wall_comment_post_form', $wall_post[$i]->nid, $user->uid)); ?>
            </div>
          </div>
          <?php
            endif;
              // End outer if.
          ?>
       <!-- write a comment Block: Ends here ! -->
      </div>  

    <!-- Wall right content Block : starts here ! -->
    </div>
  <?php 
    endfor;
  ?>
</div>
<!-- Drupal wall block : ends here ! -->
