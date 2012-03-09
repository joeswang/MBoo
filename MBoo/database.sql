DROP TABLE config;
DROP TABLE video;
DROP TABLE tutor;
DROP TABLE series;

CREATE TABLE config(
param INT,
idx    INT,
intval    INT,
charval   CHAR(256),
desc     CHAR(256)
);
INSERT INTO config VALUES(0, 1, 0, 'http://www.boobooke.com/qv.php', 'video query url');
INSERT INTO config VALUES(1, 1, 0, 'videos', 'video directory');

CREATE TABLE tutor (
tid	CHAR(10),
ename	CHAR(128)
);
INSERT INTO tutor VALUES('TID0000000', '小布');
INSERT INTO tutor VALUES('TID0000001', '黑弓');
INSERT INTO tutor VALUES('TID0000002', '赶星');
INSERT INTO tutor VALUES('TID0000003', '小辉');

CREATE TABLE series (
sid	CHAR(10),
tid	CHAR(10),
total	INT,
title	CHAR(256),
summary CHAR(2048)
);
INSERT INTO series VALUES('SID0000001', 'TID0000000', 76, 'Oracle 9i Fundamentals I系列培训视频', 'Oracle 9i培训专用'); 
INSERT INTO series VALUES('SID0000002', 'TID0000003', 40, 'MySQL菜鸟入门系列培训视频', 'MySQL入门极好的培训视频'); 

CREATE TABLE video (
vid 	CHAR(13),
tid 	CHAR(10),
sid	CHAR(10),
idx	INT,
title	CHAR(256),
summary	CHAR(2048),
tag	CHAR(128),
size	CHAR(1)
);
INSERT INTO video VALUES('bbk1109', 'TID0000000', 'SID0000001', 1, 'Oracle在Linux平台下的安装', NULL, 'Oracle|数据库', 'S');
INSERT INTO video VALUES('bbk1110', 'TID0000000', 'SID0000001', 2, 'Oracle体系结构概述', NULL, 'Oracle|数据库', 'S');
INSERT INTO video VALUES('bbk4791', 'TID0000003', 'SID0000002', 1, 'MySQL在Linux平台下的安装', NULL, 'MySQL|数据库|Linux', 'S');
INSERT INTO video VALUES('bbk4798', 'TID0000003', 'SID0000002', 2, 'MySQL体系结构概述', NULL, 'MySQL|数据库|Linux', 'S');


