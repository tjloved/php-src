--TEST--
Bug #44913 (Segfault when using return in combination with nested loops and continue 2)
--FILE--
<?php

function something()
{
    foreach (array(1, 2) as $value) {
        for ($i = 0; $i < 1; $i++) {
            continue 2;
        }
        return;
    }
}
function fn2143090423()
{
    something();
    echo "ok\n";
}
fn2143090423();
--EXPECT--
ok
