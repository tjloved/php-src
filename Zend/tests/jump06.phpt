--TEST--
jump 06: goto to undefined label
--FILE--
<?php

function fn441616315()
{
    goto L1;
}
fn441616315();
--EXPECTF--
Fatal error: 'goto' to undefined label 'L1' in %sjump06.php on line %d
