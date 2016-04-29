<?php

/**
 * @file
 * Contains \Drupal\logman\Form\LogmanStatisticsForm.
 */

namespace Drupal\logman\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class LogmanStatisticsForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'logman_statistics_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    // Add the required JS and CSS.
    $google_chart_api_url = \Drupal::config('logman.settings')->get('logman_google_chart_api_url');
    // @FIXME
    // The Assets API has totally changed. CSS, JavaScript, and libraries are now
    // attached directly to render arrays using the #attached property.
    // 
    // 
    // @see https://www.drupal.org/node/2169605
    // @see https://www.drupal.org/node/2408597
    // drupal_add_js($google_chart_api_url, array('type' => 'external', 'scope' => 'header'));

    // @FIXME
    // The Assets API has totally changed. CSS, JavaScript, and libraries are now
    // attached directly to render arrays using the #attached property.
    // 
    // 
    // @see https://www.drupal.org/node/2169605
    // @see https://www.drupal.org/node/2408597
    // drupal_add_js(drupal_get_path('module', 'logman') . '/js/logman_chart.js');

    // @FIXME
    // The Assets API has totally changed. CSS, JavaScript, and libraries are now
    // attached directly to render arrays using the #attached property.
    // 
    // 
    // @see https://www.drupal.org/node/2169605
    // @see https://www.drupal.org/node/2408597
    // drupal_add_css(drupal_get_path('module', 'logman') . '/css/logman.css');


    // Include the log operation class.
    module_load_include('php', 'logman', 'includes/lib/LogmanWatchdogSearch');
    module_load_include('php', 'logman', 'includes/lib/LogmanApacheSearch');

    // Get watchdog statistics and add to JS settings array.
    $settings = [
      'logmanStatistics' => [
        'watchdogPlaceholder' => 'watchdog_chart',
        'apachePlaceholder' => 'apache_chart',
        'watchdogTablePlaceholder' => 'watchdog_table',
        'apacheTablePlaceholder' => 'apache_table',
        'watchdogDataSelector' => 'watchdog_data',
        'apacheDataSelector' => 'apache_data',
      ]
      ];
    $watchdog_against_options = ['severity', 'type'];
    $watchdog = new LogmanWatchdogSearch();
    foreach ($watchdog_against_options as $against) {
      $watchdog_statistics_raw = $watchdog->getStatistics(NULL, $against);
      $settings['logmanStatistics'][$against] = logman_prapare_chartable_data($watchdog_statistics_raw, 'watchdog', $against);
    }

    // Get apache statistics.
    $apache = new LogmanApacheSearch(\Drupal::config('logman.settings')->get('logman_apache_log_path'));
    if ($apache->checkApacheLogPath() === TRUE) {
      $apache_against_options = ['code', 'method'];
      foreach ($apache_against_options as $against) {
        $apache_statistics = $apache->getStatistics(NULL, $against);
        $settings['logmanStatistics'][$against] = logman_prapare_chartable_data($apache_statistics, 'apache', $against);
      }
    }
    else {
      // @FIXME
// l() expects a Url object, created from a route name or external URI.
// drupal_set_message(t('Apache access log path either empty or not valid. !path', array('!path' => l(t('Please provide a valid apache access log path.'), 'admin/settings/logman'))));

    }

    // Add the JS settings array.
    // @FIXME
    // The Assets API has totally changed. CSS, JavaScript, and libraries are now
    // attached directly to render arrays using the #attached property.
    // 
    // 
    // @see https://www.drupal.org/node/2169605
    // @see https://www.drupal.org/node/2408597
    // drupal_add_js($settings, 'setting');


    $form['statistics'] = [
      '#type' => 'fieldset',
      '#title' => t('Logman Statistics'),
    ];

    $form['statistics']['watchdog'] = [
      '#type' => 'fieldset',
      '#title' => t('Watchdog'),
      '#tree' => TRUE,
      '#suffix' => '<div class="logman_clear"></div>',
    ];

    $form['statistics']['watchdog']['against'] = [
      '#type' => 'select',
      '#options' => [
        'severity' => t('Severity'),
        'type' => t('Type'),
      ],
      '#default_value' => 'severity',
    ];

    // Watchdog chart and data table.
    $form['statistics']['watchdog']['chart'] = [
      '#markup' => '<br /><div id="watchdog_chart"></div>'
      ];

    $form['statistics']['watchdog']['table'] = [
      '#markup' => '<div id="watchdog_table"></div>',
      '#prefix' => '<div class="log_data_table"><b><u>' . t('Chart Data') . '</u></b>',
      '#suffix' => '</div>',
    ];

    $form['statistics']['apache'] = [
      '#type' => 'fieldset',
      '#title' => t('Apache'),
      '#tree' => TRUE,
      '#suffix' => '<div class="logman_clear"></div>',
    ];

    // Display chart only if apache access log path is correctly set.
    if ($apache->checkApacheLogPath() === TRUE) {
      $form['statistics']['apache']['against'] = [
        '#type' => 'select',
        '#options' => [
          'code' => t('Response Code'),
          'method' => t('Method'),
        ],
        '#default_value' => 'code',
      ];

      // Apache chart and data table.
      $form['statistics']['apache']['chart'] = [
        '#markup' => '<div id="apache_chart"></div>'
        ];

      $form['statistics']['apache']['table'] = [
        '#markup' => '<div id="apache_table"></div>',
        '#prefix' => '<div class="log_data_table"><b><u>' . t('Chart Data') . '</u></b>',
        '#suffix' => '</div>',
      ];
    }
    else {
      $form['statistics']['apache']['path_error'] = [
        '#markup' => t('Apache access log path is wrong or not set.')
        ];
    }

    return $form;
  }

}
