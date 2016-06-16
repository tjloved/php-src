--TEST--
Bug #61000 (Exceeding max nesting level doesn't delete numerical vars)
--INI--
max_input_nesting_level=2
--POST--
1[a][]=foo&1[a][b][c]=bar
--GET--
a[a][]=foo&a[a][b][c]=bar
--FILE--
<?php

function fn1717077822()
{
    print_r($_GET);
    print_r($_POST);
}
fn1717077822();
--EXPECTF--
Array
(
)
Array
(
)
