<?php

/**
 * @file
 * Contains \Drupal\logman\Form\LogmanWatchdogDetailForm.
 */

namespace Drupal\logman\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class LogmanWatchdogDetailForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'logman_watchdog_detail_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
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

    $form['log_detail'] = [
      '#type' => 'fieldset',
      '#title' => t('Watchdog Log Detail'),
    ];

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
      $form['log_detail'][$field] = [
        '#value' => '<div class="log_field">' . ucwords($field) . ': </div><div class="log_field_value">' . $field_value . '</div>',
        '#prefix' => '<div class = "log_detail_item">',
        '#suffix' => '<div class = "logman_clear"></div></div>',
      ];
    }

    return $form;
  }

}
