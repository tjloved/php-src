--TEST--
Optimization of constant switch expression
--FILE--
<?php

function fn1148775483()
{
    try {
        switch ("1" . (int) 2) {
            case 12:
                throw new Exception();
        }
    } catch (Exception $e) {
        echo "exception\n";
    }
}
fn1148775483();
--EXPECT--
exception
