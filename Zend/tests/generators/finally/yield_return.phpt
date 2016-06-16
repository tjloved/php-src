--TEST--
try { yield } finally { return }
--FILE--
<?php

function foo($f, $t)
{
    for ($i = $f; $i <= $t; $i++) {
        try {
            (yield $i);
        } finally {
            return;
        }
    }
}
function fn1532692804()
{
    foreach (foo(1, 5) as $x) {
        echo $x, "\n";
    }
}
fn1532692804();
--EXPECT--
1
