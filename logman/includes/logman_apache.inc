<?php
/**
 * @file
 * Logman Apache Search UI.
 */

/**
 * Implements hook_form().
 */
function logman_apache_search_form($form, &$form_state) {
  // Add the required css.
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


  // Build form_state values from $_GET.
  // Not ideal but drupal pagination works with query string.
  $field_keys = array(
    'http_method',
    'http_response_code',
    'ip',
    'url',
    'date_from',
    'date_to',
  );
  logman_prepare_form_state($field_keys, $form_state);

  $option = array(
    'method' => !empty($form_state['values']['http_method']) ? $form_state['values']['http_method'] : 'GET',
    'code' => !empty($form_state['values']['http_response_code']) ? $form_state['values']['http_response_code'] : 200,
    'ip' => !empty($form_state['values']['ip']) ? trim($form_state['values']['ip']) : '',
    'url' => !empty($form_state['values']['url']) ? trim($form_state['values']['url']) : '',
    'date_from' => !empty($form_state['values']['date_from']) ? $form_state['values']['date_from'] : NULL,
    'date_to' => !empty($form_state['values']['date_to']) ? $form_state['values']['date_to'] : NULL,
  );

  // Field set container for search form.
  $form['apache_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Apache Search'),
    '#prefix' => '<div class="form_container">',
    '#suffix' => '</div><div class="logman_clear"></div>',
  );

  $form['apache_search']['ip'] = array(
    '#type' => 'textfield',
    '#title' => t('IP'),
    '#size' => 20,
    '#default_value' => (isset($option['ip'])) ? $option['ip'] : '',
    '#prefix' => '<div>',
  );

  $form['apache_search']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#size' => 50,
    '#default_value' => (isset($option['url'])) ? $option['url'] : '',
  );

  $form['apache_search']['http_method'] = array(
    '#type' => 'select',
    '#options' => array(
      'GET' => t('GET'),
      'POST' => t('POST'),
      'PUT' => t('PUT'),
      'DELETE' => t('DELETE'),
    ),
    '#default_value' => $option['method'],
    '#title' => t('HTTP method'),
  );

  $form['apache_search']['http_response_code'] = array(
    '#type' => 'select',
    '#options' => array(
      '100' => t('100'),
      '200' => t('200'),
      '301' => t('301'),
      '302' => t('302'),
      '404' => t('404'),
    ),
    '#default_value' => $option['code'],
    '#title' => t('HTTP response'),
    '#suffix' => '</div><div class="logman_clear"></div>',
  );

  $form['apache_search']['date_from'] = array(
    '#type' => 'date_popup',
    '#title' => t('From'),
    '#date_format' => 'Y-m-d H:i:s',
    '#date_year_range' => '-0:+0',
    '#default_value' => $option['date_from'],
    '#prefix' => '<div><div class="date_range">',
    '#suffix' => '</div>',
  );

  $form['apache_search']['date_to'] = array(
    '#type' => 'date_popup',
    '#title' => t('To'),
    '#date_format' => 'Y-m-d H:i:s',
    '#date_year_range' => '-0:+0',
    '#default_value' => $option['date_to'],
    '#prefix' => '<div class="date_range">',
    '#suffix' => '</div>',
  );

  $form['apache_search']['submit'] = array(
    '#id' => 'submit_full_apache',
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  // Search result.
  $result = logman_apache_search_result($option);
  // Field set container for search result.
  $form['apache_search_result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Apache Search Result'),
    '#prefix' => '<div class="result_container">',
    '#sufix' => '</div>',
  );
  $form['apache_search_result']['result_count'] = array(
    '#markup' => '<div>' . t('Total result:') . ' <strong>' . $result['data_count'] . '</strong></div>',
  );
  $form['apache_search_result']['result'] = array(
    '#markup' => $result['data'],
  );
  return $form;
}

/**
 * Implements hook_form_submit().
 */
function logman_apache_search_form_submit($form, &$form_state) {
  unset($_GET['page']);
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper function to perform apache access log search.
 *
 * @param array $option
 *   The apache log search options array.
 *
 * @return array|string
 *   The filter and sliced array of apache log.
 */
function logman_apache_search_result($option, $quantity = 9) {
  $apache_access_log_path = \Drupal::config('logman.settings')->get('logman_apache_log_path');
  $item_per_page = \Drupal::config('logman.settings')->get('logman_log_item_per_page');

  // Include the log operation class.
  module_load_include('php', 'logman', 'includes/lib/LogmanApacheSearch');
  $apache_log = new LogmanApacheSearch($apache_access_log_path, $item_per_page);

  if ($apache_log->checkApacheLogPath() === FALSE) {
    // @FIXME
// l() expects a Url object, created from a route name or external URI.
// drupal_set_message(t('Apache access log path either empty or not valid. !path', array('!path' => l(t('Please provide a valid apache access log path.'), 'admin/settings/logman'))));

    return array(
      'data' => '',
      'data_count' => 0,
    );
  }

  // Set the apache access log read limit.
  $apache_log->setReadLimit(\Drupal::config('logman.settings')->get('logman_apache_read_limit'));

  $search_result = $apache_log->searchLog($option);
  $header = array('IP', 'Time', 'Method', 'URL', 'Response code', 'Agent');
  if ($search_result->totalCount > 0) {
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $output = theme('table', array('header' => $header, 'rows' => $search_result->data));

    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $output .= theme('pager', array('quantity' => $quantity, 'parameters' => $option));

    return array(
      'data' => $output,
      'data_count' => $search_result->totalCount,
    );
  }
  else {
    if ($search_result->data === FALSE) {
      drupal_set_message(t("Can't open apache access log file at path %logpath"), array('%logpath' => $apache_access_log_path), 'error');
    }
    return array(
      'data' => '',
      'data_count' => 0,
    );
  }
}
