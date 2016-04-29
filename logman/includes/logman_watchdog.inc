<?php
/**
 * @file
 * Logman Watchdog Search UI.
 */

/**
 * Logman watchdog search form.
 */
function logman_watchdog_search_form($form, &$form_state) {
  // Add the required css and js.
  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'logman') . '/css/logman.css', 'logman');

  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_js(drupal_get_path('module', 'logman') . '/js/logman_search.js');


  // Include the log operation class.
  module_load_include('php', 'logman', 'includes/lib/LogmanWatchdogSearch');

  // Build form_state values from $_GET.
  // Not ideal but drupal pagination works with query string.
  $field_keys = array(
    'search_key',
    'log_type',
    'severity',
    'uid',
    'location',
    'referer',
    'items_per_page',
    'date_from',
    'date_to',
  );
  logman_prepare_form_state($field_keys, $form_state);

  // Set the form action to the menu path of this page to reset
  // the filter form previous search on click of search button.
  // @FIXME
// url() expects a route name or an external URI.
// $form['#action'] = url($_GET['q']);


  // Field set container for search form.
  $form['watchdog_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Watchdog Search'),
    '#prefix' => '<div class="form_container">',
    '#suffix' => '</div><div class="logman_clear"></div>',
  );
  // Search key.
  $form['watchdog_search']['search_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Search Message'),
    '#default_value' => isset($form_state['values']['search_key']) ? $form_state['values']['search_key'] : '',
    '#prefix' => '<div>',
  );

  // Log type to search.
  $log_type_options = array('all' => t('All')) + LogmanWatchdogSearch::getLogTypes();
  $form['watchdog_search']['log_type'] = array(
    '#type' => 'select',
    '#title' => t('Log Type'),
    '#options' => $log_type_options,
    '#default_value' => isset($form_state['values']['log_type']) ? $form_state['values']['log_type'] : 'all',
  );

  // Log severity.
  $severity_options = array_map('ucwords', array('all' => 'All') + watchdog_severity_levels());
  $form['watchdog_search']['severity'] = array(
    '#type' => 'select',
    '#title' => t('Severity'),
    '#options' => $severity_options,
    '#default_value' => isset($form_state['values']['severity']) ? $form_state['values']['severity'] : 'all',
  );

  // User.
  $form['watchdog_search']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#size' => 18,
    '#default_value' => isset($form_state['values']['uid']) ? $form_state['values']['uid'] : '',
    '#suffix' => '</div><div class="logman_clear"></div>',
  );

  // Location.
  $form['watchdog_search']['location'] = array(
    '#type' => 'textfield',
    '#title' => t('Location'),
    '#default_value' => isset($form_state['values']['location']) ? $form_state['values']['location'] : '',
    '#prefix' => '<div>',
  );

  // Referrer.
  $form['watchdog_search']['referer'] = array(
    '#type' => 'textfield',
    '#title' => t('Referer'),
    '#default_value' => isset($form_state['values']['referer']) ? $form_state['values']['referer'] : '',
    '#suffix' => '</div><div class="logman_clear"></div>',
  );

  // Date range from.
  $form['watchdog_search']['date_from'] = array(
    '#type' => 'date_popup',
    '#title' => t('From'),
    '#date_format' => 'Y-m-d H:i:s',
    '#date_year_range' => '-0:+0',
    '#default_value' => !empty($form_state['values']['date_from']) ? $form_state['values']['date_from'] : NULL,
    '#prefix' => '<div><div class="date_range">',
    '#suffix' => '</div>',
  );

  // Date range to.
  $form['watchdog_search']['date_to'] = array(
    '#type' => 'date_popup',
    '#title' => t('To'),
    '#date_format' => 'Y-m-d H:i:s',
    '#date_year_range' => '-0:+0',
    '#default_value' => !empty($form_state['values']['date_to']) ? $form_state['values']['date_to'] : NULL,
    '#prefix' => '<div class="date_range">',
    '#suffix' => '</div>',
  );

  $form['watchdog_search']['submit'] = array(
    '#id' => 'submit_full_watchdog',
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  // Display the search result.
  $items_per_page = \Drupal::config('logman.settings')->get('logman_items_per_page');
  $search_result = logman_watchdog_search_result($form_state, $items_per_page);
  if (!empty($search_result['themed_result'])) {
    $form['watchdog_search_result'] = array(
      '#type' => 'fieldset',
      '#title' => t('Watchdog Search Result'),
      '#prefix' => '<div class="result_container">',
      '#sufix' => '</div>',
    );

    $form['watchdog_search_result']['srearch_result'] = array(
      '#markup' => $search_result['themed_result'],
    );

    $form['watchdog_search_result']['pagination'] = array(
      '#markup' => $search_result['result']->pagination,
    );
  }

  return $form;
}

/**
 * Logman full log search form submit handler.
 *
 * @param array $form
 *   The drupal form array.
 * @param array $form_state
 *   The drupal form state array.
 */
function logman_watchdog_search_form_submit(&$form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper function for Logman full log search.
 *
 * @param array $form_state
 *   The drupal form state array.
 * @param int $items_per_page
 *   Number of result items per page.
 * @param int $quantity
 *   Number of pagination links per page.
 *
 * @return array
 *   Array containing themed and raw search result.
 */
function logman_watchdog_search_result($form_state, $items_per_page = 10, $quantity = 9) {
  // Include the log operation class.
  module_load_include('php', 'logman', 'includes/lib/LogmanWatchdogSearch');

  // Check for the log type.
  if (isset($form_state['values'])) {
    $search_key = isset($form_state['values']['search_key']) ? $form_state['values']['search_key'] : '';
    $log_type = isset($form_state['values']['log_type']) ? $form_state['values']['log_type'] : 'all';
    if ($log_type == 'all') {
      $watchdog_log = new LogmanWatchdogSearch($search_key);
    }
    else {
      $watchdog_log = new LogmanWatchdogSearch($search_key, $log_type);
    }
  }
  else {
    $watchdog_log = new LogmanWatchdogSearch();
  }
  // Prepare the params array.
  $params = array();
  $search_fields = array('severity', 'uid', 'location', 'referer');
  foreach ($search_fields as $search_field) {
    if (isset($form_state['values'])) {
      if (isset($form_state['values'][$search_field]) && $form_state['values'][$search_field] != 'all') {
        $params[$search_field] = $form_state['values'][$search_field];
      }
    }
  }
  // Prepare the date range.
  if (!empty($form_state['values']['date_from']) && !empty($form_state['values']['date_to'])) {
    $params['date_range'] = array(strtotime($form_state['values']['date_from']), strtotime($form_state['values']['date_to']));
  }
  elseif (!empty($form_state['values']['date_from']) && empty($form_state['values']['date_to'])) {
    $params['date_range'] = array(strtotime($form_state['values']['date_from']));
  }
  else {
    $params['date_range'] = array();
  }

  $watchdog_log->setLimit($items_per_page);
  $watchdog_log->setQuantity($quantity);
  $search_result = $watchdog_log->searchLog($params);
  if (count($search_result->matches) > 0) {
    // Get the Severity levels.
    $severity_levels = watchdog_severity_levels();

    $rows = array();
    foreach ($search_result->matches as $data) {
      $replacements = unserialize($data['variables']);
      $message = $data['message'];
      if (!empty($replacements)) {
        $message = str_replace(array_keys($replacements), array_values($replacements), $data['message']);
      }
      // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $rows[] = array(
//         $data['wid'],
//         ucwords($data['type']),
//         l($message, 'admin/reports/logman/watchdog/' . $data['wid'] . '/detail',
//           array(
//             'attributes' => array(
//               'target' => '_blank',
//             ),
//           )
//         ),
//         ucwords($severity_levels[$data['severity']]),
//         $data['uid'],
//         $data['location'],
//         $data['referer'],
//         date('Y-m-d H:i:s', $data['timestamp']),
//       );

    }
    $header = array(
      t('Wid'),
      t('Type'),
      t('Message'),
      t('Severity'),
      t('User'),
      t('Location'),
      t('Referrer'),
      t('DateTime'),
    );
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $themed_result = theme('table', array('header' => $header, 'rows' => $rows));

  }
  else {
    $themed_result = t('No matches found.');
  }

  return array(
    'result' => $search_result,
    'themed_result' => $themed_result,
  );
}

/**
 * Implements hook_form().
 */
function logman_watchdog_detail_form() {
  // Add the required css.
  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'logman') . '/css/logman.css', 'logman');


  // Include the log operation class.
  module_load_include('php', 'logman', 'includes/lib/LogmanWatchdogSearch');
  $log_id = arg(4);

  // Get the Severity levels.
  $severity_levels = watchdog_severity_levels();

  $watchdog_log = new LogmanWatchdogSearch();
  $log_detail = $watchdog_log->getLogDetail($log_id);

  $form['log_detail'] = array(
    '#type' => 'fieldset',
    '#title' => t('Watchdog Log Detail'),
  );

  foreach ($log_detail as $field => $value) {
    if ($field == 'message' && !empty($log_detail['variables'])) {
      $replacements = unserialize($log_detail['variables']);
      $field_value = str_replace(array_keys($replacements), array_values($replacements), $value);
    }
    elseif ($field == 'variables' && !empty($value)) {
      $field_value = print_r(unserialize($value), TRUE);
    }
    elseif ($field == 'severity') {
      $field_value = ucwords($severity_levels[$value]);
    }
    elseif ($field == 'timestamp') {
      $field_value = date('Y-m-d H:i:s', $value);
    }
    else {
      $field_value = $value;
    }
    $form['log_detail'][$field] = array(
      '#value' => '<div class="log_field">' . ucwords($field) . ': </div><div class="log_field_value">' . $field_value . '</div>',
      '#prefix' => '<div class = "log_detail_item">',
      '#suffix' => '<div class = "logman_clear"></div></div>',
    );
  }

  return $form;
}
