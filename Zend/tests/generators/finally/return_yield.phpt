--TEST--
try { return } finally { yield }
--FILE--
<?php

function foo($f, $t)
{
    for ($i = $f; $i <= $t; $i++) {
        try {
            return;
        } finally {
            (yield $i);
        }
    }
}
function fn92187333()
{
    foreach (foo(1, 5) as $x) {
        echo $x, "\n";
    }
}
fn92187333();
--EXPECT--
1
