--TEST--
Catch first exception in the second multicatch
--FILE--
<?php

function fn1774628051()
{
    require_once __DIR__ . '/exceptions.inc';
    try {
        echo 'TRY' . PHP_EOL;
        throw new Exception3();
    } catch (Exception1|Exception2 $e) {
        echo get_class($e) . PHP_EOL;
    } catch (Exception3|Exception4 $e) {
        echo get_class($e) . PHP_EOL;
    } finally {
        echo 'FINALLY' . PHP_EOL;
    }
}
fn1774628051();
--EXPECT--
TRY
Exception3
FINALLY

