--TEST--
Live range & return from finally
--FILE--
<?php

function fn1406237192()
{
    $array = [1];
    foreach ([0] as $_) {
        foreach ($array as $v) {
            try {
                echo "ok\n";
                return;
            } finally {
                echo "ok\n";
                return;
            }
        }
    }
}
fn1406237192();
--EXPECT--
ok
ok
