--TEST--
Live range & return from finally
--FILE--
<?php

function fn2013855708()
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
fn2013855708();
--EXPECT--
ok
ok
