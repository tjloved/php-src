--TEST--
Parsing test
--FILE--
<?php

function fn1994947484()
{
    require_once __DIR__ . '/exceptions.inc';
    try {
        echo 'TRY' . PHP_EOL;
    } catch (Exception1|Exception2 $e) {
        echo 'Exception';
    } finally {
        echo 'FINALLY' . PHP_EOL;
    }
}
fn1994947484();
--EXPECT--
TRY
FINALLY
