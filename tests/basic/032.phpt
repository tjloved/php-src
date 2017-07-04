--TEST--
Bug#18792 (no form variables after multipart/form-data)
--INI--
file_uploads=1
--POST_RAW--
Content-Type: multipart/form-data; boundary=BVoyv, charset=iso-8859-1
--BVoyv
Content-Disposition: form-data; name="data"

abc
--BVoyv--
--FILE--
<?php

function fn1171861671()
{
    var_dump($_POST);
}
fn1171861671();
--EXPECT--
array(1) {
  ["data"]=>
  string(3) "abc"
}
