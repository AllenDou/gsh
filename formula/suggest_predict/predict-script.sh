#!/bin/sh
#redis-cli -h 218.30.115.72 -p 36379 script load "\
#local haha = cjson.decode(KEYS[1])
#local max_s = 0
#local m_cls = 0 
#local PA,PTA,PT= 0 
#local n_blog,all
#local wc = 0
#local score = 0
#local ret={}
#
#for k,v in pairs(haha.keyWords) do 
#	wc = wc + v.count
#end
#
#for db_n = 1,22 do
#	local P = 0
#	redis.call('select',db_n)
#	n_blog = redis.call('get','n_blog')
#	all = redis.call('get','all')
#	PA = n_blog/1000000;
#	for k,v in pairs(haha.keyWords) do 
#		score = redis.call('zscore','keyword',v.word)
#		if score then
#			PTA = score/all
#			PT = v.count/wc
#			P = P + PTA*PA/PT
#		end
#	end
#	ret[db_n] = tostring(P)
#	if P > max_s then
#		max_s = P
#		m_cls = db_n
#	end
#end
#
#return m_cls"
##1 "{\"blogid\":\"5f56a4640100md37\",\"blog_pubdate\":\"1282028281\",\"classid\":\"15\",\"body\":\"-\",\"keyWords\":[{\"word\":\"发射系统\",\"attr\":\"n\",\"count\":19,\"tf\":0.052058360161654924,\"tfidf\":0.7192128244436503},{\"word\":\"导弹\",\"attr\":\"n\",\"count\":47,\"tf\":0.06438797177888898,\"tfidf\":0.27454768431526294},{\"word\":\"军舰\",\"attr\":\"n\",\"count\":4,\"tf\":0.0054798273854373605,\"tfidf\":0.02718731559632947},{\"word\":\"改进型\",\"attr\":\"n\",\"count\":2,\"tf\":0.00410987053907802,\"tfidf\":0.026671839394623614},{\"word\":\"威力\",\"attr\":\"n\",\"count\":4,\"tf\":0.0054798273854373605,\"tfidf\":0.025432756346167536}]}"


redis-cli -h 218.30.115.72 -p 36379 evalsha 687d905934497518b7260978f511609e62040973 1 "{\"blogid\":\"5f56a4640100md37\",\"blog_pubdate\":\"1282028281\",\"classid\":\"15\",\"body\":\"-\",\"keyWords\":[{\"word\":\"发射系统\",\"attr\":\"n\",\"count\":19,\"tf\":0.052058360161654924,\"tfidf\":0.7192128244436503},{\"word\":\"导弹\",\"attr\":\"n\",\"count\":47,\"tf\":0.06438797177888898,\"tfidf\":0.27454768431526294},{\"word\":\"军舰\",\"attr\":\"n\",\"count\":4,\"tf\":0.0054798273854373605,\"tfidf\":0.02718731559632947},{\"word\":\"改进型\",\"attr\":\"n\",\"count\":2,\"tf\":0.00410987053907802,\"tfidf\":0.026671839394623614},{\"word\":\"威力\",\"attr\":\"n\",\"count\":4,\"tf\":0.0054798273854373605,\"tfidf\":0.025432756346167536}]}"
