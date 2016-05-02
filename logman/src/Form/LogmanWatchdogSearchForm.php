<?php

/**
 * @file
 * Contains \Drupal\logman\Form\LogmanWatchdogSearchForm.
 */

namespace Drupal\logman\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupa\logman\Helper\LogmanWatchdogSearch;

class LogmanWatchdogSearchForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'logman_watchdog_search_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    // Add the required css and js.
    $form['#attached']['library'][] = 'logman/logman-report';

    // Build form_state values from $_GET.
    // Not ideal but drupal pagination works with query string.
    $field_keys = [
      'search_key',
      'log_type',
      'severity',
      'uid',
      'location',
      'referer',
      'items_per_page',
      'date_from',
      'date_to',
    ];
    logman_prepare_form_state($field_keys, $form_state);

    // Set the form action to the menu path of this page to reset
    // the filter form previous search on click of search button.
    // @FIXME
    // url() expects a route name or an external URI.
    // $form['#action'] = url($_GET['q']);


    // Field set container for search form.
    $form['watchdog_search'] = [
      '#type' => 'fieldset',
      '#title' => t('Watchdog Search'),
      '#prefix' => '<div class="form_container">',
      '#suffix' => '</div><div class="logman_clear"></div>',
    ];
    // Search key.
    $form['watchdog_search']['search_key'] = [
      '#type' => 'textfield',
      '#title' => t('Search Message'),
      '#default_value' => !$form_state->getValue([
        'search_key'
        ]) ? $form_state->getValue(['search_key']) : '',
      '#prefix' => '<div>',
    ];

    // Log type to search.
    $log_type_options = ['all' => t('All')] + LogmanWatchdogSearch::getLogTypes();
    $form['watchdog_search']['log_type'] = [
      '#type' => 'select',
      '#title' => t('Log Type'),
      '#options' => $log_type_options,
      '#default_value' => !$form_state->getValue([
        'log_type'
        ]) ? $form_state->getValue(['log_type']) : 'all',
    ];

    // Log severity.
    $severity_options = array_map('ucwords', [
      'all' => 'All'
      ] + watchdog_severity_levels());
    $form['watchdog_search']['severity'] = [
      '#type' => 'select',
      '#title' => t('Severity'),
      '#options' => $severity_options,
      '#default_value' => !$form_state->getValue([
        'severity'
        ]) ? $form_state->getValue(['severity']) : 'all',
    ];

    // User.
    $form['watchdog_search']['uid'] = [
      '#type' => 'textfield',
      '#title' => t('User'),
      '#size' => 18,
      '#default_value' => !$form_state->getValue([
        'uid'
        ]) ? $form_state->getValue(['uid']) : '',
      '#suffix' => '</div><div class="logman_clear"></div>',
    ];

    // Location.
    $form['watchdog_search']['location'] = [
      '#type' => 'textfield',
      '#title' => t('Location'),
      '#default_value' => !$form_state->getValue([
        'location'
        ]) ? $form_state->getValue(['location']) : '',
      '#prefix' => '<div>',
    ];

    // Referrer.
    $form['watchdog_search']['referer'] = [
      '#type' => 'textfield',
      '#title' => t('Referer'),
      '#default_value' => !$form_state->getValue([
        'referer'
        ]) ? $form_state->getValue(['referer']) : '',
      '#suffix' => '</div><div class="logman_clear"></div>',
    ];

    // Date range from.
    $form['watchdog_search']['date_from'] = [
      '#type' => 'date_popup',
      '#title' => t('From'),
      '#date_format' => 'Y-m-d H:i:s',
      '#date_year_range' => '-0:+0',
      '#default_value' => $form_state->getValue([
        'date_from'
        ]) ? $form_state->getValue(['date_from']) : NULL,
      '#prefix' => '<div><div class="date_range">',
      '#suffix' => '</div>',
    ];

    // Date range to.
    $form['watchdog_search']['date_to'] = [
      '#type' => 'date_popup',
      '#title' => t('To'),
      '#date_format' => 'Y-m-d H:i:s',
      '#date_year_range' => '-0:+0',
      '#default_value' => $form_state->getValue([
        'date_to'
        ]) ? $form_state->getValue(['date_to']) : NULL,
      '#prefix' => '<div class="date_range">',
      '#suffix' => '</div>',
    ];

    $form['watchdog_search']['submit'] = [
      '#id' => 'submit_full_watchdog',
      '#type' => 'submit',
      '#value' => t('Search'),
    ];

    // Display the search result.
    $items_per_page = \Drupal::config('logman.settings')->get('logman_items_per_page');
    $search_result = logman_watchdog_search_result($form_state, $items_per_page);
    if (!empty($search_result['themed_result'])) {
      $form['watchdog_search_result'] = [
        '#type' => 'fieldset',
        '#title' => t('Watchdog Search Result'),
        '#prefix' => '<div class="result_container">',
        '#sufix' => '</div>',
      ];

      $form['watchdog_search_result']['srearch_result'] = [
        '#markup' => $search_result['themed_result']
        ];

      $form['watchdog_search_result']['pagination'] = [
        '#markup' => $search_result['result']->pagination
        ];
    }

    return $form;
  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $form_state->setRebuild(TRUE);
  }

}
