--TEST--
jump 06: goto to undefined label
--FILE--
<?php

function fn244513268()
{
    goto L1;
}
fn244513268();
--EXPECTF--
Fatal error: 'goto' to undefined label 'L1' in %sjump06.php on line %d
