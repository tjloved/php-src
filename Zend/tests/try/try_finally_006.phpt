--TEST--
Try finally (with near goto)
--FILE--
<?php

function foo()
{
    $jmp = 1;
    try {
    } finally {
        previous:
        if ($jmp) {
            goto label;
            echo "dummy";
            label:
            echo "label\n";
            $jmp = 0;
            goto previous;
        }
        echo "okey";
    }
}
function fn1990548167()
{
    foo();
}
fn1990548167();
--EXPECTF--
label
okey
