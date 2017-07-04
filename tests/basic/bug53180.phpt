--TEST--
Bug #53180 (post_max_size=0 partly not working)
--INI--
post_max_size=0
--POST--
email=foo&password=bar&submit=Log+on
--FILE--
<?php

function fn909341430()
{
    var_dump($_POST);
}
fn909341430();
--EXPECT--
array(3) {
  ["email"]=>
  string(3) "foo"
  ["password"]=>
  string(3) "bar"
  ["submit"]=>
  string(6) "Log on"
}
