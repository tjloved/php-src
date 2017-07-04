--TEST--
Catch last exception in the multicatch
--FILE--
<?php

function fn750356886()
{
    require_once __DIR__ . '/exceptions.inc';
    try {
        echo 'TRY' . PHP_EOL;
        throw new Exception3();
    } catch (Exception1|Exception2|Exception3 $e) {
        echo get_class($e) . PHP_EOL;
    } finally {
        echo 'FINALLY' . PHP_EOL;
    }
}
fn750356886();
--EXPECT--
TRY
Exception3
FINALLY
