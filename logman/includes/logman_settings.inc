<?php
/**
 * @file
 * Logman Admin Settings.
 * Defines configuration settings for logman.
 */

/**
 * Configure the logman settings.
 */
function logman_settings_form($form_state) {
  $form = array();
  if (isset($form_state['storage']['confirm']) && $form_state['storage']['confirm'] == TRUE) {
    $message = t('Are you sure you want to reset graylog2/gelf for case insensitivity?');
    $description = t('If you choose yes this will delete the current logged messages to add new case insensitive mapping.
                      Please ensure you backup the logs before you reset.');
    return confirm_form($form, $message, 'admin/settings/logman', $description, t('Yes'), t('No'));
  }

  $form['logman_watchdog_log_type'] = array(
    '#type' => 'select',
    '#options' => array(
      'dblog' => t('Dblog'),
      'gelf' => t('Gelf'),
    ),
    '#title' => t('Watchdog log type'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_watchdog_log_type'),
    '#description' => t("Please select the watchdog log type as Dblog or Gelf."),
    '#required' => TRUE,
  );

  // Add additional settings for using GELF.
  if (\Drupal::config('logman.settings')->get('logman_watchdog_log_type') == 'gelf') {
    $form['logman_gelf_host'] = array(
      '#type' => 'textfield',
      '#title' => t('Host for graylog2 server'),
      '#default_value' => \Drupal::config('logman.settings')->get('logman_gelf_host'),
      '#description' => t("Please provide graylog2 server's host."),
      '#required' => TRUE,
    );

    $form['logman_gelf_port'] = array(
      '#type' => 'textfield',
      '#title' => t('Port for graylog2 server'),
      '#default_value' => \Drupal::config('logman.settings')->get('logman_gelf_port'),
      '#description' => t("Please provide graylog2 server's port."),
      '#required' => TRUE,
    );

    $form['logman_gelf_node'] = array(
      '#type' => 'textfield',
      '#title' => t('Node for graylog2 server'),
      '#default_value' => \Drupal::config('logman.settings')->get('logman_gelf_node'),
      '#description' => t("Please provide graylog2 server's node."),
      '#required' => TRUE,
    );

    // Apply case insensitive search for graylo2/GELF logs.
    $form['logman_gelf_reset'] = array(
      '#type' => 'submit',
      '#value' => t("Apply Case Insensitive Search for GELF"),
      '#submit' => array('logman_settings_form_gelf_reset'),
    );
  }

  $form['logman_apache_log_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Apache access log path'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_apache_log_path'),
    '#description' => t("Please provide your server's apache access log path. It will be something like /var/log/httpd/access_log"),
    '#required' => TRUE,
  );

  $form['logman_apache_read_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Apache access log read limit'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_apache_read_limit'),
    '#description' => t("Please the limit of character length t you would like to process the apache access log file."),
    '#required' => TRUE,
  );

  $form['logman_items_per_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Log item per page'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_items_per_page'),
    '#description' => t("Please provide item per page, must be greater or equal to 1."),
    '#required' => TRUE,
  );

  $form['logman_show_page_statistics'] = array(
    '#type' => 'textfield',
    '#title' => t('Show logman page statistics'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_show_page_statistics'),
    '#description' => t("The will display logman page statistics on each page if user has access logman permission. Use '1' to display and '0'  to hide."),
    '#size' => 1,
  );

  $form['logman_page_statistics_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Logman page statistics duration'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_page_statistics_duration'),
    '#description' => t("The duration of logman page statistics like since 10 days or 10 hours."),
    '#size' => 10,
  );

  $form['logman_page_statistics_duration_unit'] = array(
    '#type' => 'select',
    '#title' => t('Logman page statistics duration unit'),
    '#options' => array(
      'hours' => t('Hours'),
      'days' => t('Days'),
    ),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_page_statistics_duration_unit'),
    '#description' => t("The unit of duration of logman page statistics, like days or hours."),
  );

  $form['logman_google_chart_api_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Chart API URL'),
    '#default_value' => \Drupal::config('logman.settings')->get('logman_google_chart_api_url'),
    '#description' => t("The google chart API URL"),
    '#size' => 100,
  );

  return system_settings_form($form);
}

/**
 * Defining the validation of the settings form.
 */
function logman_settings_form_validate($form, &$form_state) {
  $log_type = $form_state['values']['logman_watchdog_log_type'];
  $path = trim($form_state['values']['logman_apache_log_path']);
  $read_limit = trim($form_state['values']['logman_apache_read_limit']);
  $item_per_page = trim($form_state['values']['logman_items_per_page']);

  if ($form_state['values']['op'] == t('Save configuration')) {
    if (!\Drupal::moduleHandler()->moduleExists($log_type)) {
      $replacement = array('@log_type' => $log_type);
      form_set_error('logman_watchdog_log_type', t("The watchdog log type selected needs the @log_type module to be enabled.", $replacement));
    }
    if ($log_type == 'gelf' && !\Drupal::moduleHandler()->moduleExists('elastic_search_clients')) {
      form_set_error('logman_watchdog_log_type', t("For using logman with gelf you need to have the module elastic_search_clients enabled."));
    }
    if (!is_readable($path)) {
      form_set_error('logman_apache_log_path', t("The file in apache access log path either doesn't exist or is not readable."));
    }
    if ($item_per_page <= 0) {
      form_set_error('logman_items_per_page', t('Item per page should be greater or equal to 1.'));
    }
    if ($read_limit < 1000) {
      form_set_error('logman_apache_read_limit', t('For proper performance of apache access log search and statistics please enter a value greater than or equal to 1000.'));
    }
  }
}

/**
 * Submit handler for logman settings form.
 */
function logman_settings_form_submit($form, &$form_state) {
  // Reset graylog2 mapping if user has confirmed.
  if ($form_state['storage']['confirm'] == TRUE) {
    // Include the graylog2/gelf operation class.
    module_load_include('php', 'logman', 'includes/lib/LogmanGraylogSearch');
    // Reset the graylog2/gelf log for case insensitive search.
    LogmanGraylogSearch::applyCaseInsensitiveSearch();
    drupal_set_message(t('Graylog2 has been reset for case insensitive search, this has deleted existing log data. Now you can restore the backed up logs.'));
    $form_state['storage']['confirm'] = FALSE;
  }
}

/**
 * Submit handler for graylog2/gelf log reset.
 */
function logman_settings_form_gelf_reset(&$form, &$form_state) {
  $form_state['storage']['confirm'] = TRUE;
}
