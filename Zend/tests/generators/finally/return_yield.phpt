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
function fn1725762984()
{
    foreach (foo(1, 5) as $x) {
        echo $x, "\n";
    }
}
fn1725762984();
--EXPECT--
1
