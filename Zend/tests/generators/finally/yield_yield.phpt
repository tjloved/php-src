--TEST--
Try { yield } finally { yield }
--FILE--
<?php

function foo()
{
    try {
        echo "1";
        (yield "2");
        echo "3";
    } finally {
        echo "4";
        (yield "5");
        echo "6";
    }
    echo "7";
}
function fn700103790()
{
    foreach (foo() as $x) {
        echo $x;
    }
}
fn700103790();
--EXPECT--
1234567
