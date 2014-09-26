--TEST--
IntlTimeZone::hasSameRules(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');

try { var_dump($tz->hasSameRules('foo')); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }

try { var_dump(intltz_has_same_rules(null, $tz)); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }

--EXPECT--
EngineException: Argument 1 passed to IntlTimeZone::hasSameRules() must be an instance of IntlTimeZone, string given
EngineException: Argument 1 passed to intltz_has_same_rules() must be an instance of IntlTimeZone, null given
