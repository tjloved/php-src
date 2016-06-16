--TEST--
Simple Switch Test
--FILE--
<?php

function fn1060183669()
{
    $a = 1;
    switch ($a) {
        case 0:
            echo "bad";
            break;
        case 1:
            echo "good";
            break;
        default:
            echo "bad";
            break;
    }
}
fn1060183669();
--EXPECT--
good
