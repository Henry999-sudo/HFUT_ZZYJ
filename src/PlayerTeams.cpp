/*
Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Modified by ming gao Hfut..for server V14
1). dash dirction for not only forward and back but also left right and other 45*(-3,-1,1,3) dirction,,
2). add Stamina.Capacity and the senceHandle analy systerm and worldmodel update

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file PlayerTeams.cpp
<pre>
<b>File:</b>          PlayerTest.cpp
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Created:</b>       10/12/2000
<b>Last Revision:</b> $ID$
<b>Contents:</b>      This file contains the class definitions for the
                      Player that are used to test the teams' high level
                      strategy.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
10/12/2000        Jelle Kok       Initial version created
</pre>
*/

#include "Player.h"

/*!This method is the first complete simple team and defines the actions taken
   by all the players on the field (excluding the goalie). It is based on the
   high-level actions taken by the simple team FC Portugal that it released in
   2000. The players do the following:
   - if ball is kickable
       kick ball to goal (random corner of goal)
   - else if i am fastest player to ball
       intercept the ball
   - else
       move to strategic position based on your home position and pos ball */
///misol.gao fixed--->
///大家好！很高兴你们找到了这个函数！这个函数，就是我们球队决策的起点，所有的比赛策略都在这里，从这里出发，不会错的！
/// deMeer5 是球员的策略函数！ 在下面你们会发现 deMeer5_goalie 那个是守门员的，目前基本可以忽略。
SoccerCommand Player::deMeer5(  )
{

    SoccerCommand soc(CMD_ILLEGAL);// 定义一个命令对象，后面会频繁用它来构造一个球员动作命令
    VecPosition   posAgent = WM->getAgentGlobalPosition();// 一个点对象，表示Agent(可以理解成当前在执行这个代码的球员它自己)的位置
    ///下面我们把“我“就理解成当前执行这个代码的Agent，因为有11个球员在执行这个代码，所以对于每个球员 肯定有一个“我”存在。
    VecPosition   posBall  = WM->getBallPos();// 球的位置
    VecPosition   posGoal  = WM->getPosOpponentGoal();// 球的位置
    int           iTmp;


    if ( WM->isBeforeKickOff( ) ) /// 如果还没有开球 注意！ 这里只是还没有开球的情况！ 要做开球后的决策，向下面继续找另外一个 WM->isBeforeKickOff( )
    {
        if ( WM->isKickOffUs( ) && WM->getPlayerNumber() == 9 ) // 9 takes kick //就找9号去开球（判断了是不是我们开球，和我"Agent"是不是9号）
        {
            if ( WM->isBallKickable() )// 当球对于我来说是不是可踢！
            {
		    VecPosition posGoal(PITCH_LENGTH/2.0, 
				       (-1 + 2*(WM->getCurrentCycle()%2)) * 
				       0.4 * SS->getGoalWidth());
		    soc=kickTo( posGoal,SS->getBallSpeedMax());// 朝球门方向将球以最大力度踢
// 		else 
// 		{
// 		    if(WM->getAgentObjectType()==OBJECT_TEAMMATE_9 )//|| 
// 		      //WM->getAgentObjectType()==OBJECT_TEAMMATE_8 || 
// 		      //WM->getAgentObjectType()==OBJECT_TEAMMATE_7) 
// 		    {
// 		      soc=dribble(0.0,DRIBBLE_SLOW); //带球 
// 		    }
// 		    else 
// 		      soc=leadingPass(OBJECT_TEAMMATE_9,1);//传球 		
// 		}
// 		ACT->putCommandInQueue(soc); 
// 		ACT->putCommandInQueue(turnNeckToObject(OBJECT_BALL,soc));

            }
            else //如果球对于我不可踢，那我就去抢球！
            {
                soc = intercept( false );
                Log.log( 100, "move to ball to take kick-off" );
            }
            ACT->putCommandInQueue( soc );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            return soc;
        }
        //下面不用管！
        if ( formations->getFormation() != FT_INITIAL || // not in kickoff formation
                posAgent.getDistanceTo( WM->getStrategicPosition() ) > 2.0 )
        {
            formations->setFormation( FT_INITIAL );       // go to kick_off formation
            ACT->putCommandInQueue( soc=teleportToPos( WM->getStrategicPosition() ));
        }
        else                                            // else turn to center
        {
            ACT->putCommandInQueue( soc=turnBodyToPoint( VecPosition( 0, 0 ), 0 ) );
            ACT->putCommandInQueue( alignNeckWithBody( ) );
        }
    }
    
    /*
		л�ɳ� 

*/    
    else if( WM->isCornerKickUs())//我方角球
    {
	if(WM->isBallKickable() )
	{
	    soc = kickTo(pointKickToC(), 2.0);
	}
	else if(WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL, &iTmp)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(posBall, PS->getPlayerWhenToTurnAngle());
	}
	else if(WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES, OBJECT_BALL)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(pointKickToC(), PS->getPlayerWhenToTurnAngle());
	}
	else
	{
	    soc = moveToPos(WM->getStrategicPosition(), PS->getPlayerWhenToTurnAngle());
	}
	
	ACT->putCommandInQueue( soc ); // 放入命令队列
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    else if( WM->isCornerKickThem() )//对方角球
    {
	ObjectT agent = WM->getAgentObjectType();
	ObjectT oppSecNearBall = WM->getSecondClosestInSetTo( OBJECT_SET_OPPONENTS, OBJECT_BALL );
	if( posBall.getY() < 0)
	{
	    if( agent==OBJECT_TEAMMATE_2 || agent==OBJECT_TEAMMATE_3 )
		soc = moveToPos( WM->getGlobalPosition( oppSecNearBall ), PS->getPlayerWhenToTurnAngle() );
	}
	else
	{
	    if( agent==OBJECT_TEAMMATE_4 || agent==OBJECT_TEAMMATE_5 )
		soc = moveToPos( WM->getGlobalPosition( oppSecNearBall ), PS->getPlayerWhenToTurnAngle() );
	}
	ACT->putCommandInQueue( soc );
	ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    
    else if( WM->isKickInUs() )//我方界外球
    {
	if( WM->isBallKickable() )
	{
	    ObjectT o = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES,WM->getPlayerType());
	      if (WM->isSafeO(-15,15,WM->getGlobalPosition(o))){
		  soc = leadingPass(o,1);
	      }else{
		  soc = leadingPass(o,3);
	      }
	}
	else if(WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL, &iTmp)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(posBall, PS->getPlayerWhenToTurnAngle());
	}
	else if(WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES, OBJECT_BALL)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(pointKickToK(), PS->getPlayerWhenToTurnAngle());
	}
	else
	{
	    soc = moveToPos(WM->getStrategicPosition(), PS->getPlayerWhenToTurnAngle());
	}
	
	ACT->putCommandInQueue( soc ); // 放入命令队列
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    } 
    
    else if (WM->isKickInThem()){	//对方边界球
	ObjectT o1,o2;
	o1 = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_BALL);
	o2 = WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_BALL);
	if (WM->getPlayerType() == o1){
	  VecPosition pos = WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,OBJECT_BALL);
	  pos.setX(pos.getX()-2);
	  pos.setY(pos.getY()+2);
	  soc = moveToPos(pos,PS->getPlayerWhenToKick());
	}else if( WM->getPlayerType()==o2){
	  VecPosition pos = WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,OBJECT_BALL);
	  pos.setX(pos.getX()+3);
	  pos.setY(pos.getY()+2);
	  soc = moveToPos(pos,PS->getPlayerWhenToKick());
	}else{
	  VecPosition pos = WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,WM->getPlayerType());
	  soc = moveToPos(pos,PS->getPlayerWhenToKick());
	}
	ACT->putCommandInQueue( soc ); // 放入命令队列
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    else if( WM->isFreeKickUs() )//我方任意球
    {
	if( WM->isBallKickable() )
	{
	    if(WM->isInTheirPenaltyArea(WM->getBallPos()))//x大于35直接射门
	    {
		VecPosition posGoalie=WM->getGlobalPosition(WM->getOppGoalieType());
		AngDeg ang_goalie=(posGoalie-posAgent).getDirection();
		AngDeg angup=(VecPosition(52.5,6.0)-posAgent).getDirection();
		AngDeg angdown=(VecPosition(52.5,-6.0)-posAgent).getDirection();

		if(fabs(angup-ang_goalie)>fabs(angdown-ang_goalie))
		    soc=kickTo(VecPosition(52.5,6.0),SS->getBallSpeedMax());
		else 
		    soc=kickTo(VecPosition(52.5,-6.0),SS->getBallSpeedMax());
	    }
	    else
	    {
		soc = kickTo(pointKickToK(), 2.0);		
	    }

	}
	else if(WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL, &iTmp)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(posBall, PS->getPlayerWhenToTurnAngle());
	}
	else if(WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES, OBJECT_BALL)
	  == WM->getAgentObjectType())
	{
	    soc = moveToPos(pointKickToK(), PS->getPlayerWhenToTurnAngle());
	}
	else
	{
	    soc = moveToPos(WM->getStrategicPosition(), PS->getPlayerWhenToTurnAngle());
	}

	ACT->putCommandInQueue( soc ); // 放入命令队列
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    else if(WM->isFreeKickThem())//对方任意球
    {
	VecPosition vec = WM->getBallPos();
	VecPosition v = vec;
	v.setX(v.getX()+5);
        if(WM->isInOwnPenaltyArea(WM->getBallPos())){
	   if (WM->getPlayerType()==OBJECT_TEAMMATE_11){
	      v.setY(v.getY()-3);
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_8){
	      v.setY(v.getY()-1);
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_9){
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_7){
	       v.setY(v.getY()+1);
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_6){
	       v.setY(v.getY()+3);
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else{
	      ObjectT o = WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,WM->getPlayerType());
	      soc = moveToPos(o, PS->getPlayerWhenToTurnAngle());
	  }
	    
	}else{
	      if(WM->getPlayerType()==OBJECT_TEAMMATE_9){
	      v.setY(v.getY()-1);
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_7){
	      soc = moveToPos(v, PS->getPlayerWhenToTurnAngle());
	  }else if(WM->getPlayerType()==OBJECT_TEAMMATE_8){
	      soc = moveToPos(pointKickToK(), PS->getPlayerWhenToTurnAngle());
	  }else{
	      ObjectT o = WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,WM->getPlayerType());
	      soc = moveToPos(o, PS->getPlayerWhenToTurnAngle());
	  }
	ACT->putCommandInQueue( soc ); // 放入命令队列
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
      }
      
    }
    else if (WM->isOffsideUs()){	//我方边线球
	if (WM->isBallKickable()){
	    ObjectT o = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES,posAgent);
	      if (WM->isSafeO(-15,15,WM->getGlobalPosition(o))){
		  soc = leadingPass(o,1);
	      }else{
		  soc = leadingPass(o,3);
	      }
	}
	else if(WM->getAgentObjectType() == WM->getClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_BALL))
	{
	    soc = moveToPos(pointKickToK(), PS->getPlayerWhenToTurnAngle());
	}
	else if(posAgent == WM->getGlobalPosition(WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_BALL))){
	    VecPosition vec = WM->getBallPos();
	    if (vec.getX()>0)
		vec.setX(vec.getX()+3);
	    else 
		vec.setX(vec.getX()-5);
	    if (vec.getY()>0)
	      vec.setY(vec.getY()-5);
	    else 
	      vec.setY(vec.getY()+8);
	    soc = moveToPos(vec,PS->getPlayerWhenToTurnAngle());
	}
      
    }else if( WM->isOffsideThem() )	//对方边线球
         {
             ObjectT o = WM->getSecondClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	    
             if( WM->getAgentObjectType() == o )
             {
                 ObjectT opp = WM->getSecondClosestInSetTo( OBJECT_SET_OPPONENTS, OBJECT_BALL );
                 soc = moveToPos( WM->getGlobalPosition( opp ), PS->getPlayerWhenToTurnAngle() );   
             }
             ObjectT o2 = WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
             ObjectT agent = WM->getAgentObjectType();
             if( agent!=o2&&agent!=o && WM->getGlobalPosition( agent ).getX()>0 )
                 soc = moveToPos( posAgent+VecPosition( -10, 0 ), PS->getPlayerWhenToTurnAngle() );
             ACT->putCommandInQueue( soc );
             ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
         }
         /*
	  * Xie Songchen
	  */
    
    
    else  /// 这个else 大家应该可以知道，是开球以后的决策！ 也就是最重要的比赛正常模式下的决策！ 很重要，我们就从这里开始！！！！ Start！
    {
        formations->setFormation( FT_343_ATTACKING);//设置球队出场阵形！
        soc.commandType = CMD_ILLEGAL;//初始化soc命令对象
        
        /* Zyj 11.12
	 */
	ObjectT o;
	VecPosition ballPos = WM->getGlobalPosition(OBJECT_BALL);
	/////////////////////////////////////////
        if ( WM->getConfidence( OBJECT_BALL ) < PS->getBallConfThr() )//判断对球的可信度，如果小于某个阈值，则...也就是说，如果很多周期没有看到球在哪里了
        {
            ACT->putCommandInQueue( soc = searchBall() );   // if ball pos unknown //执行找球动作！并放入命令队列
            ACT->putCommandInQueue( alignNeckWithBody( ) ); // search for it //同时把脖子随身体一起转
        }
        
        else if ( WM->isBallKickable())                   // if kickable // 如果球已知，而且当前球在我脚下(可踢)
        {
	  
	  /////////////////////////////
	  
	  if(WM->isFreeKickUs())//带球绕人
	  {
	      double ang = (VecPosition(52.5,0)-posAgent).getDirection();            
	      if ( WM->isOpponentAtAngleEx(ang-45, ang, 6) ) 
		ang+=45; 
	      else if ( WM->isOpponentAtAngleEx(ang,ang+45,6) ) 
		ang-=45; 
	       soc = kickTo ( ang , SS->getBallSpeedMax()  );   
	  }
	  
	  /*
	   * 
	   * else if ( WM->getBallPos().getX() < -25 )//在本方半场
	    {
		 if( WM->getPlayerNumber() == 2 || WM->getPlayerNumber() == 5 )
                    {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        ObjectT opp = WM->getClosestRelativeInSet(OBJECT_SET_OPPONENTS);
                        if(WM->getGlobalPosition(opp).getDistanceTo(posAgent) < 5) //如果周围没人
                        {
                            soc = clearBall( CLEAR_BALL_DEFENSIVE );
                        }
                        else 
                        { 
                            if(WM->getPlayerNumber()==2)
                            {
				if(WM->NumAround(OBJECT_SET_OPPONENTS,5,WM->getGlobalPosition(OBJECT_TEAMMATE_10)) < WM->NumAround(OBJECT_SET_OPPONENTS,5,WM->getGlobalPosition(OBJECT_TEAMMATE_7))) 
				{
				    soc = leadingPass(OBJECT_TEAMMATE_10,WM->getGlobalPosition(OBJECT_TEAMMATE_10).getDistanceTo(posAgent));
				}
				else 
				{
				     soc = leadingPass(OBJECT_TEAMMATE_7,WM->getGlobalPosition(OBJECT_TEAMMATE_7).getDistanceTo(posAgent));
				}
			    }
                            else if(WM->getPlayerNumber()==7)
                            {
                                if(WM->NumAround(OBJECT_SET_OPPONENTS,5,WM->getGlobalPosition(OBJECT_TEAMMATE_11)) < WM->NumAround(OBJECT_SET_OPPONENTS,5,WM->getGlobalPosition(OBJECT_TEAMMATE_8)))
				{
				    soc = leadingPass(OBJECT_TEAMMATE_11,WM->getGlobalPosition(OBJECT_TEAMMATE_11).getDistanceTo(posAgent));
				}
				else 
				{
				    soc = leadingPass(OBJECT_TEAMMATE_8,WM->getGlobalPosition(OBJECT_TEAMMATE_8).getDistanceTo(posAgent));
				}
                            }   
                            //soc = clearBall(CLEAR_BALL_DEFENSIVE);
                        }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                    
                    }
                    if( WM->getPlayerNumber() == 6 || WM->getPlayerNumber() == 7 )
                    {
// /*		    if( WM->getPlayerNumber() == 7 )
// 			soc = directPass( VecPosition(-10, 25), PASS_FAST);
// 		    else
// 			soc = directPass( VecPosition(-10, -25), PASS_FAST);		
                        soc = clearBall( CLEAR_BALL_DEFENSIVE );
                    }
                    if( WM->getPlayerNumber() == 10 || WM->getPlayerNumber() == 11 )
                    {
                        if( WM->getPlayerNumber() == 10 )
                            soc = directPass( VecPosition(10, 25), PASS_FAST);
                        else
                            soc = directPass( VecPosition(10, -25), PASS_FAST);			
                    }
		
                    else
                    {
                            if( posAgent.getY() > 0) 
                                soc = directPass( VecPosition( posAgent.getX() + 20, 25), PASS_FAST);
                            else
                                soc = directPass( VecPosition( posAgent.getX() + 20, -25), PASS_FAST);
                    }    
	    }*/
	  
	  else if (WM->getBallPos().getX() < -27 )//在本方半场情况危险
	  {
	    if ( WM->getPlayerNumber() == 2)             //清球出门
	    {
	      
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_10))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_10,1.0);
	      }
	 
		soc = clearBall( CLEAR_BALL_DEFENSIVE );
	    
	    }
	    
	    else if (WM->getPlayerNumber() == 5)
	    {
	      
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_11))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_11,1.0);
	      }
	      else
		soc = clearBall( CLEAR_BALL_DEFENSIVE );
	      
	    }
	    
	    else if( WM->getPlayerNumber() == 7 )
	    {	
		if (WM->isSafeForPass(OBJECT_TEAMMATE_10))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_10,1.0);
	      }
	      else  if (WM->isSafeForPass(OBJECT_TEAMMATE_11))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_11,1.0);
	      }
	      else
		soc = clearBall( CLEAR_BALL_DEFENSIVE );
	    }
	    else if (WM->getPlayerNumber() == 6)
	    {
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_10))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_10,1.0);
	      }
	      else  if (WM->isSafeForPass(OBJECT_TEAMMATE_11))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_11,1.0);
	      }
	      else
		soc = clearBall( CLEAR_BALL_DEFENSIVE );
	    }
	    
	    else if (WM->getPlayerNumber() == 10)
	    {
	     VecPosition pos = VecPosition(10, 20);
	     soc = directPass(pos,PASS_FAST);
	    }
	    
	    else if (WM->getPlayerNumber() == 11)
	    {
	     VecPosition pos = VecPosition(10, -20);
	     soc = directPass(pos,PASS_FAST);
	    }
	    else if (WM->getPlayerNumber() == 8)
	    {
	     soc = dribble((posAgent-WM->getPosOpponentGoal()).getDirection(),DRIBBLE_FAST);
	    }
	    else if (WM->getPlayerNumber() == 9)
	    {
	      soc = dribble((posAgent-WM->getPosOpponentGoal()).getDirection(),DRIBBLE_FAST);
	    }
	    else if (WM->getPlayerNumber() == 7)
	    {
	     soc = dribble((posAgent-WM->getPosOpponentGoal()).getDirection(),DRIBBLE_FAST);
	    }
	    else                //其他球员
	    {
	      if (posAgent.getY() > 0)
	      {
		soc = directPass(VecPosition(posAgent.getX() + 20, 20), PASS_FAST);
	      }
	      else
	      {
		soc = directPass(VecPosition(posAgent.getX() + 20, 20), PASS_FAST);
	      }
	    }
	  }
	  else if ( WM->getBallPos().getX() > -27 && WM->getBallPos().getX() < -5 )//在本方半场情况稍微好点
	  {
	    
	    if ( WM->getPlayerNumber() == 2)             //找机会向边路传
	    {
	      int a = WM->getCurrentCycle()%2;
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_8))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_8,1.0);
	      }
	      else if (WM->isSafeForPass(OBJECT_TEAMMATE_10))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_10,1.0);
	      }
	      else
	      {
		Circle cir(posAgent, 7.0);
		int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir);
		/*
		if (num >= 2)
		{
		  //soc = clearBall( CLEAR_BALL_DEFENSIVE );
		}
		else
		{
		  soc = directPass(VecPosition(posAgent.getX() + 20, -20), PASS_FAST);
		}*/
		soc = kickTo(VecPosition(0,15),SS->getBallSpeedMax()/2);
	      }
	    }
	    
	    else if (WM->getPlayerNumber() == 5)
	    {
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_9))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_9,1.0);
	      }
	      else if (WM->isSafeForPass(OBJECT_TEAMMATE_11))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_11,1.0);
	      }
	      else
	      {
		Circle cir(posAgent, 7.0);
		int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir);
		/*
		if (num >= 2)
		{
		  soc = clearBall( CLEAR_BALL_DEFENSIVE );
		}
		else
		{
		  soc = directPass(VecPosition(posAgent.getX() + 20, -20), PASS_FAST);
		}*/
		soc = kickTo(VecPosition(0,-15),SS->getBallSpeedMax()/2);
	      }
	    }
	    
	    else if (WM->getPlayerNumber() == 10)
	    {
	      /*
	     VecPosition pos = VecPosition(10, 20);
	     soc = directPass(pos,PASS_FAST);*/
	      soc = dribble((posAgent-WM->getPosOpponentGoal()).getDirection(),DRIBBLE_FAST);
	    }
	    
	    else if (WM->getPlayerNumber() == 11)
	    {
	      /*
	     VecPosition pos = VecPosition(10, -20);
	     soc = directPass(pos,PASS_FAST);*/
	      soc = dribble((posAgent-WM->getPosOpponentGoal()).getDirection(),DRIBBLE_FAST);
	    }
	    
	    else       //找机会传边路
	    {
	      if (WM->isSafeForPass(OBJECT_TEAMMATE_11)&&
		  (WM->getAgentGlobalPosition().getDistanceTo(WM->getGlobalPosition(OBJECT_TEAMMATE_11)) < 8))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_11,1.0);
	      }
	      else if (WM->isSafeForPass(OBJECT_TEAMMATE_10)&&
		  (WM->getAgentGlobalPosition().getDistanceTo(WM->getGlobalPosition(OBJECT_TEAMMATE_10)) < 8))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_10,1.0);
	      }
	      else 
	      {
		if (posAgent.getY() > 0)
		{
		  soc = directPass(VecPosition(posAgent.getX() + 20, 20), PASS_FAST);
		}
		else
		{
		  soc = directPass(VecPosition(posAgent.getX() + 20, 20), PASS_FAST);
		}
	      }
	    }
	    
	    
	  }
	  /*
	  else if (WM->getBallPos().getX() < 10 && WM->getBallPos().getX() >= -5)
	  {
	    if (WM->getPlayerNumber() == 9)
	    {
	      double ang = (VecPosition(52.5,0)-posAgent).getDirection();            
	      if ( WM->isOpponentAtAngleEx(ang-45, ang, 6) ) 
		ang+=45; 
	      else if ( WM->isOpponentAtAngleEx(ang,ang+45,6) ) 
		ang-=45; 
	       soc = dribble(ang, DRIBBLE_FAST);
	    }
	    
	    if (WM->getPlayerNumber() == 7 || WM->getPlayerNumber() == 8)
	    {
	      if (WM->isSafeForPass(WM->getGlobalPosition(OBJECT_TEAMMATE_9)))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_9,1.0);
	      }
	      else
	      {
		double ang = (VecPosition(52.5,0)-posAgent).getDirection();            
		if ( WM->isOpponentAtAngleEx(ang-45, ang, 6) ) 
		  ang+=45; 
		else if ( WM->isOpponentAtAngleEx(ang,ang+45,6) ) 
		  ang-=45; 
		soc = dribble(ang, DRIBBLE_FAST);
	      }
	    }
	    
	    if (WM->getPlayerNumber() == 6)
	    {
	      if (WM->isSafeForPass(WM->getGlobalPosition(OBJECT_TEAMMATE_9)))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_9,1.0);
	      }
	      else if (WM->isSafeForPass(WM->getGlobalPosition(OBJECT_TEAMMATE_7)) )
	      {
		soc = leadingPass(OBJECT_TEAMMATE_7,1.0);
	      }
	       else if (WM->isSafeForPass(WM->getGlobalPosition(OBJECT_TEAMMATE_8)))
	      {
		soc = leadingPass(OBJECT_TEAMMATE_8,1.0);
	      }
	      else
	      {
		double ang = (VecPosition(52.5,0)-posAgent).getDirection();            
		if ( WM->isOpponentAtAngleEx(ang-45, ang, 6) ) 
		  ang+=45; 
		else if ( WM->isOpponentAtAngleEx(ang,ang+45,6) ) 
		  ang-=45; 
		soc = dribble(ang, DRIBBLE_FAST);
	      }
	    }
	    
	    
	  }*/
	  else if ( WM->getBallPos().getX() < 10 && WM->getBallPos().getX() >= -5 )//在本方半场
	  {

		if( WM->getPlayerNumber() == 10 || WM->getPlayerNumber() == 11 )
		{
		    if( WM->getPlayerNumber() == 10 )
			soc = dribble_Dodge( VecPosition(35, 30), posAgent);
		    else
			soc = dribble_Dodge( VecPosition(35, -30), posAgent);			
		}
		
		else if (WM->getPlayerNumber() == 6||WM->getPlayerNumber()== 8) 
		{
		  Circle cir(WM->getGlobalPosition(OBJECT_TEAMMATE_7),7.0);
		  int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir);
		    if (num < 2)
		    {
		     soc = throughPass(OBJECT_TEAMMATE_7,posGoal); 
		    }
		    else if(WM->getBallPos().getY() < 0&&WM->getGlobalPosition(OBJECT_TEAMMATE_11).getX()>posAgent.getX())
			soc = leadingPass( OBJECT_TEAMMATE_11 , 1.5 );
		    else if(WM->getBallPos().getY() < 0&&WM->getGlobalPosition(OBJECT_TEAMMATE_11).getX()>posAgent.getX())
			soc = leadingPass( OBJECT_TEAMMATE_10 , 1.5 );
		    else
		    {
		      soc = kickTo(VecPosition(25,18),SS->getBallSpeedMax());
		    }
		    
		}
		else if (WM->getPlayerNumber() == 7)
		{
		  ObjectT temp = WM->getAgentObjectType();
		  int iIndex;
		 for(ObjectT o = WM->iterateObjectStart(iIndex,OBJECT_SET_TEAMMATES);o != OBJECT_ILLEGAL;o = WM->iterateObjectNext(iIndex,OBJECT_SET_TEAMMATES))
		 {
		   if (WM->getGlobalPosition(temp).getX()< WM->getGlobalPosition(o).getX())
		   {
		    temp = o; 
		   }
		 }
		 WM->iterateObjectDone(iIndex);
		  if (WM->isSafeForPass(temp))
		  {
		   soc = throughPass(temp,posGoal); 
		  }
		  else{
		   soc = dribble_Dodge(VecPosition(25,18),posAgent); 
		  }
		}
		    
	  }
	     
	    else if( WM->getBallPos().getX() >= 7 && WM->getBallPos().getX() <= 45)//进入后半场
	    {
	         int num =WM->opponentInAngle((posAgent-VecPosition(52.0,6.5)).getDirection(),(posAgent-VecPosition(52.0,-6.5)).getDirection());
		 if (posBall.getX() > 30&&fabs(posBall.getY()) < 15)
		 {
		   double ang = (VecPosition(52.5,0)-posAgent).getDirection();            
		  if ( WM->isOpponentAtAngleEx(ang-25, ang, 6) ) 
		    ang+=25; 
		  else if ( WM->isOpponentAtAngleEx(ang,ang+25,6) ) 
		    ang-=25;
		  
		  if (WM->getGlobalPosition(WM->getOppGoalieType()).getY() > 0)
		  {
		    soc = kickTo(VecPosition(52.0,-SS->getGoalWidth()/2+2),SS->getBallSpeedMax());/////////////
		  }
		  else
		  {
		    soc = kickTo(VecPosition(52.0,SS->getGoalWidth()/2-2),SS->getBallSpeedMax());/////////////
		  }
		  
		 }
	      /////
		 if( WM->getPlayerNumber() == 11 )//边路选手11
		 {
		    Circle cir1( posAgent, 6 );
		    int num=WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);
		    
		    if(num >= 2 && !WM->isInTheirPenaltyArea( WM->getBallPos()) && posAgent.getX() < 35 )//有人阻拦且没到禁区
		    {
			    if (WM->getAgentEffort()< 4000)
			    {
			     soc = leadingPass(OBJECT_TEAMMATE_8,1.0); 
			    }
			    else
			    {
			      
			      
			      ObjectT temp = OBJECT_TEAMMATE_8;
				
				if( WM->getGlobalPosition(temp).getX() > 
				WM->getGlobalPosition(OBJECT_TEAMMATE_6).getX() &&
					WM->getGlobalPosition( temp ).getX() > 
				posAgent.getX() - 10 ) //8号比6靠前,且不远
				{
					soc = throughPass( temp , posGoal );
				}
				
				else if(posAgent.getDistanceTo( WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, posAgent) ) 
				< posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_TEAMMATE_6)) )
				{
					soc = leadingPass( WM->getClosestInSetTo(OBJECT_SET_TEAMMATES , posAgent) ,1 );			      
				}
				
				else if( WM->getGlobalPosition( OBJECT_TEAMMATE_6 ).getX() > 
				posAgent.getX() - 10 ) //6号离得butai远
				{
					soc = leadingPass( OBJECT_TEAMMATE_6 ,1 );
				}
				
				else
				soc = maxAngShoot1(posAgent);
			      
			    }
				
			
		    }
		    
// 		    else if( WM->isInTheirPenaltyArea( WM->getBallPos()) && 
// 		      WM->isInTheirPenaltyArea( WM->getGlobalPosition(OBJECT_TEAMMATE_8)) )
// 		    {
// 			double ang = (VecPosition(52.5,0)-posAgent).getDirection();
// 			if( WM-> isOpponentAtAngleEx(ang-15, ang + 15, 15) )
// // 			if( posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_OPPONENT_1)) < 7);
// 			    //soc = leadingPass( OBJECT_TEAMMATE_8 , 1);
// 			  soc = clearBall( CLEAR_BALL_GOAL );
// 			else
// 			    soc = maxAngShoot1( posAgent);
// 		    }

		    else if (fabs(WM->getBallPos().getY())>13 && WM->getBallPos().getX() > 45 
			    && posAgent.getDistanceTo(WM->getGlobalPosition(WM->getClosestRelativeInSet(OBJECT_SET_OPPONENTS)))<3)
		    {
		      ObjectT temp1 = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, WM->getOppGoalieType());
		      if (WM->getAgentObjectType() != temp1)
		      {
			soc = leadingPass(temp1,(posAgent-temp1).getDirection());
		      }
		      else
		      {
			ObjectT temp2 = WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES, WM->getOppGoalieType());
			soc = leadingPass(temp2,(posAgent-temp2).getDirection());
		      }
		      
		    }
		    else if( WM->isInTheirPenaltyArea( WM->getBallPos()) && fabs( WM->getBallPos().getY()) < 13 )//进入禁区射门
		    {
			
			double ang = (VecPosition(52.5,0)-posAgent).getDirection();
			if( WM-> isOpponentAtAngleEx(ang-15, ang + 15, 15) && 
			      posAgent.getDistanceTo(WM->getGlobalPosition(WM->getOppGoalieType())) < 5 || WM->isOpponentAtAngle(WM->getAgentGlobalVelocity().getDirection(),7) )
			{			    
			    soc = leadingPass( WM->getClosestInSetTo( OBJECT_SET_TEAMMATES , posAgent ), 1);				  
			}

			else
			{
			    soc = maxAngShoot(posAgent);
			}
				  
		    }
		    
		    
		    else//往攻击阵位移动
		    {
				VecPosition posGoal = WM->getPosOpponentGoal();
				VecPosition atkPos( posGoal.getX() - 6, posGoal.getY() - 15 );//攻击阵位
				
				if( WM->getPlayerNumber() == 10 )
				{
				  atkPos.setY( posGoal.getY() + 15 ); 
				}
				
				soc = dribble_Dodge( atkPos , posAgent );
		    }
		 }
		 
		 else if( WM->getPlayerNumber() == 10 )//边路选手10
		 {
		    Circle cir1( posAgent, 6 );
		    int num=WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);
		    
		    if(num >= 2 && !WM->isInTheirPenaltyArea( WM->getBallPos()) &&posAgent.getX() < 35)//有人阻拦且没到禁区
		    {	
			if (WM->getAgentEffort()< 4000)
			    {
			     soc = leadingPass(OBJECT_TEAMMATE_8,1.0); 
			    }
			    else
			    {
			      
			      
			      ObjectT temp = OBJECT_TEAMMATE_8;
				
				if( WM->getGlobalPosition(temp).getX() > 
				WM->getGlobalPosition(OBJECT_TEAMMATE_6).getX() &&
					WM->getGlobalPosition( temp ).getX() > 
				posAgent.getX() - 10 ) //8号比6靠前,且不远
				{
					soc = throughPass( temp , posGoal );
				}
				
				else if(posAgent.getDistanceTo( WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, posAgent) ) 
				< posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_TEAMMATE_6)) )
				{
					soc = leadingPass( WM->getClosestInSetTo(OBJECT_SET_TEAMMATES , posAgent) ,1 );			      
				}
				
				else if( WM->getGlobalPosition( OBJECT_TEAMMATE_6 ).getX() > 
				posAgent.getX() - 10 ) //6号离得butai远
				{
					soc = leadingPass( OBJECT_TEAMMATE_6 ,1 );
				}
				
				else
				soc = maxAngShoot1(posAgent);
			      
			    }
			    
		    }
		    
// 		    else if( WM->isInTheirPenaltyArea( WM->getBallPos()) &&  
// 		      WM->isInTheirPenaltyArea( WM->getGlobalPosition(OBJECT_TEAMMATE_9)))
// 		    {
// 			double ang = (VecPosition(52.5,0)-posAgent).getDirection();
// 			if( WM-> isOpponentAtAngleEx(ang-15, ang + 15, 15) )
// 			//if( posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_OPPONENT_1)) < 7)			
// 			    //soc = leadingPass( OBJECT_TEAMMATE_9 , 1);
// 			    soc = clearBall( CLEAR_BALL_GOAL );
// 			else
// 			    soc = maxAngShoot1( posAgent);
// 		    }
 		    
		    else if( WM->isInTheirPenaltyArea( WM->getBallPos()) )//进入禁区射门
		    {
			double ang = (VecPosition(52.5,0)-posAgent).getDirection();
			if( WM-> isOpponentAtAngleEx(ang-15, ang + 15, 15) && 
			      posAgent.getDistanceTo(WM->getGlobalPosition(WM->getOppGoalieType())) < 5 || WM->isOpponentAtAngle(WM->getAgentGlobalVelocity().getDirection(),7) )
			{			    
			    soc = leadingPass( WM->getClosestInSetTo( OBJECT_SET_TEAMMATES , posAgent ), 1);				  
			}

			else
			{
			    soc = maxAngShoot(posAgent);
			}
		    }
		    else if (fabs(WM->getBallPos().getY())>13 && WM->getBallPos().getX() > 40 
			    && posAgent.getDistanceTo(WM->getGlobalPosition(WM->getClosestRelativeInSet(OBJECT_SET_OPPONENTS)))<3)
		    {
		       ObjectT temp1 = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, WM->getOppGoalieType());
		      if (WM->getAgentObjectType() != temp1)
		      {
			soc = leadingPass(OBJECT_TEAMMATE_7,(posAgent-temp1).getDirection());
		      }
		      else
		      {
			ObjectT temp2 = WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES, WM->getOppGoalieType());
			soc = leadingPass(temp2,(posAgent-temp1).getDirection());
		      }
		    }
		    else//往攻击阵位移动
		    {
			VecPosition posGoal = WM->getPosOpponentGoal();
			VecPosition atkPos( posGoal.getX() - 6, posGoal.getY() - 15 );//攻击阵位
			
			if( WM->getPlayerNumber() == 10 )
			{
			    atkPos.setY( posGoal.getY() + 15 ); 
			}
			
			soc = dribble_Dodge( atkPos , posAgent );			
		    }
		 }
		 
		 else if( WM->getPlayerNumber() == 6 )//接应选手6
		 {
		    Circle cir1( posAgent, 6 );
		    int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
		    
		    
		    if(num >= 2 &&  WM-> isOpponentAtAngleEx(ang-20, ang + 20, 15))
		    {			
			ObjectT temp = OBJECT_TEAMMATE_9;
			
			Circle cir1( WM->getGlobalPosition(temp), 6 );
			int num1 = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    

			Circle cir2( WM->getGlobalPosition(OBJECT_TEAMMATE_8), 6 );
			int num2 = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);	
			
			if( num1 > num2  )
			    soc = leadingPass( OBJECT_TEAMMATE_8 , 1);
			else if( num1 < num2)
			    soc = leadingPass( temp , 1);
			else
			    soc = leadingPass( OBJECT_TEAMMATE_11 ,1);
		    }

		    else if( WM->getBallPos().getX() < 35 )
		    {
			soc = dribble_Dodge( VecPosition(35, -10) , posAgent );			
		    }
		    
		    else
			soc = maxAngShoot( posAgent );//直接射门		 
		}
		 
		 else if( WM->getPlayerNumber() == 7 )//接应选手7
		 {
		    Circle cir1( posAgent, 6 );
		    int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
			
		    if(num >= 2 &&  WM-> isOpponentAtAngleEx(ang-20, ang + 20, 15))
		    {			
			ObjectT temp = OBJECT_TEAMMATE_9;
			
			Circle cir1( WM->getGlobalPosition(temp), 6 );
			int num1 = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    

			Circle cir2( WM->getGlobalPosition(OBJECT_TEAMMATE_8), 6 );
			int num2 = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);	
			
			if( num1 > num2  )
			    soc = leadingPass( OBJECT_TEAMMATE_8 , 1);
			else if( num1 < num2 )
			    soc = leadingPass( temp , 1);
			
			else 
			    soc = leadingPass( OBJECT_TEAMMATE_10 ,1);
	    
		    }

		    else if( WM->getBallPos().getX() < 30 )
		    {
			soc =maxAngShoot(posAgent);		
		    }
		    
		    else
			soc = maxAngShoot( posAgent );//直接射门		 
		}
		
		else if( WM->getPlayerNumber() == 9 || WM->getPlayerNumber() == 8 )//突击手8,9
		{
		    Circle cir1( posAgent, 6 );
		    int num = WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);	
		    
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
		    
		    if( WM->isOpponentAtAngleEx( ang-15, ang+15, 15) //用来应对球门球 5.14
		      && WM->getBallPos().getX() > 33
		      && fabs( WM->getBallPos().getY() ) < 16)
		    {
			soc = maxAngShoot(posAgent); 
		    }
		    
		    else if(num >= 3)
		    {
		        if(posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_TEAMMATE_10)) //有人抢，回传
			  < posAgent.getDistanceTo( WM->getGlobalPosition(OBJECT_TEAMMATE_11)) )
			    soc = leadingPass( OBJECT_TEAMMATE_10 ,1 );
			else
			    soc = leadingPass( OBJECT_TEAMMATE_11 ,1 );
		    }
		    
		    else if( WM->getBallPos().getX() < 30 )
			soc = dribble_Dodge( WM->getPosOpponentGoal() , posAgent );
		    else
		    {
		     VecPosition posGoal( PITCH_LENGTH/2.0,
				 (-1 + 2*(WM->getCurrentCycle()%2)) * 0.4 * SS->getGoalWidth() ); //定义一个射门点 PITCH_LENGTH是球场的长度。这个点就是按照周期看，交替的打球门的两个死角。
			
			soc = kickTo( posGoal, SS->getBallSpeedMax() ); // kick maximal // 把球以最大速度踢向那个射门点*///直接射门
		    }
		
		}
		 
		 else 
		 {
		  VecPosition posGoal( PITCH_LENGTH/2.0,
				 (-1 + 2*(WM->getCurrentCycle()%2)) * 0.4 * SS->getGoalWidth() ); //定义一个射门点 PITCH_LENGTH是球场的长度。这个点就是按照周期看，交替的打球门的两个死角。
		  soc = kickTo( posGoal, SS->getBallSpeedMax() ); // kick maximal // 把球以最大速度踢向那个射门点*/ 
		 }
	    }

	    else if( WM->getBallPos().getX() >= 45 &&
		     WM->getBallPos().getX() < 52)//球距离球门比较近了 
	    {
		 if( WM->getPlayerNumber() == 9 || WM->getPlayerNumber() == 8 )//主要突破选手
		 {
		    Circle cir1( posAgent, 6 );
		    int num=WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
		    
		    if( WM->isOpponentAtAngleEx( ang-15, ang+15, 15))
		    {
		        soc = directPass( WM->getGlobalPosition( 
		        WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, posAgent) ), PASS_FAST);
		    }
		    else soc = maxAngShoot( posAgent );
		 }
		 
		 else if( WM->getPlayerNumber() == 10 )
		 {
		    Circle cir1( posAgent, 3 );
		    int num=WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
		    if( WM-> isOpponentAtAngleEx(ang-15, ang +15, 15) )
		    {
			    soc = leadingPass( OBJECT_TEAMMATE_8 , 1);			
		    }
		    if(num >= 1)
		    {
		        if(WM->getGlobalPosition(OBJECT_TEAMMATE_7).getX() 
			  < WM->getGlobalPosition(OBJECT_TEAMMATE_9).getX() )
			    soc = directPass( WM->getGlobalPosition( OBJECT_TEAMMATE_9 ), PASS_FAST);	
			else
			    soc = directPass( WM->getGlobalPosition( OBJECT_TEAMMATE_7 ), PASS_FAST);	
		    }
		    else soc = maxAngShoot1( posAgent );
		 }
		 
		 else if( WM->getPlayerNumber() == 11 )
		 {
		    Circle cir1( posAgent, 3 );
		    int num=WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS,cir1);		    
		    double ang = (VecPosition(52.5,0)-posAgent).getDirection();
		    
		    if( WM-> isOpponentAtAngleEx(ang-15, ang + 15, 15) )
		    {
			    soc = leadingPass( OBJECT_TEAMMATE_9 , 1);			
		    }
		    else if(num >= 1)
		    {
		        if(WM->getGlobalPosition(OBJECT_TEAMMATE_6).getX() 
			  < WM->getGlobalPosition(OBJECT_TEAMMATE_8).getX() )
			    soc = directPass( WM->getGlobalPosition( OBJECT_TEAMMATE_8 ), PASS_FAST);	
			else
			    soc = directPass( WM->getGlobalPosition( OBJECT_TEAMMATE_6 ), PASS_FAST);	
		    }
		    else soc = maxAngShoot( posAgent );
		 }
		 
		 else soc = maxAngShoot1( posAgent );
	    }
	  else if (posBall.getX()<0)
	  {
	    if (WM->getPlayerNumber() == 7)
	    {
	     if (WM->isSafeForPass(OBJECT_TEAMMATE_10))
	     {
	      soc = leadingPass(OBJECT_TEAMMATE_10,1.0); 
	     }
	     else if (WM->isSafeForPass(OBJECT_TEAMMATE_9))
	     {
	       soc= leadingPass(OBJECT_TEAMMATE_9,0.0);
	     }
	     else
	     {
	      soc = kickTo(VecPosition(posAgent.getX()+10,posAgent.getY()),SS->getBallSpeedMax()/5); 
	     }
	    }
	    else if (WM->getPlayerNumber() == 8 || WM->getPlayerNumber() == 9)
	    {
	      
	      if (WM->getPlayerNumber() == 8)
	      {
		soc= dribble_Dodge(VecPosition(22,-13),posAgent	);
	      }
	      else{
		soc= dribble_Dodge(VecPosition(22, 13),posAgent	);
	      }
	      
	    }
	    else if (WM->getPlayerNumber() == 10 || WM->getPlayerNumber() == 11)
	    {
	      if (WM->getPlayerNumber() == 11)
	      {
		soc = dribble_Dodge(VecPosition(20,-20),posAgent);
	      }
	      else
	      {
		soc = dribble_Dodge(VecPosition(20,20),posAgent);
	      }
	      
	    }
	    else{
	      
	      ObjectT teammate = WM->getClosestRelativeInSet(OBJECT_SET_TEAMMATES);
	      
	     if (WM->isSafeForPass(teammate) && WM->getGlobalPosition(teammate).getX()-5>posAgent.getX())
	     {
	      soc = leadingPass(teammate,1.0); 
	     }
	     else
	     {
	       soc = kickTo(VecPosition(10,0),SS->getBallSpeedMax()/5);
	     }
	    }
	  }
	  else
	  {
	    VecPosition posGoal( PITCH_LENGTH/2.0,
				 (-1 + 2*(WM->getCurrentCycle()%2)) * 0.4 * SS->getGoalWidth() ); //定义一个射门点 PITCH_LENGTH是球场的长度。这个点就是按照周期看，交替的打球门的两个死角。
		soc = kickTo( posGoal, SS->getBallSpeedMax() ); // kick maximal // 把球以最大速度踢向那个射门点*/
	  }
	    
	  ACT->putCommandInQueue( soc ); // 放入命令队列
	  ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) ); // 把脖子转向球，也就是一直看着球
	  Log.log( 100, "kick ball" );		  
	  ////////////////////////////////////
	}
	/*
	else if (WM->isHoldBallThem(o)&&ballPos.getX() < 0)
	{
	  /*ObjectT Agent = WM->getAgentObjectType();
		int index;
		for (ObjectT teammate = WM->iterateObjectStart(index,OBJECT_SET_TEAMMATES);teammate!=OBJECT_ILLEGAL;
			  teammate=WM->iterateObjectNext(index,OBJECT_SET_TEAMMATES))
		{
		 
		}
	}*/
        else if ( WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL, &iTmp )
                  == WM->getAgentObjectType()  && !WM->isDeadBallThem() ) // 如果球不在我的控制范围下，但是当前能最快抢到球的是我，那我就去执行抢球动作
        {                                                // if fastest to ball
            Log.log( 100, "I am fastest to ball; can get there in %d cycles", iTmp );
            soc = intercept( false );                      // intercept the ball

            if ( soc.commandType == CMD_DASH &&            // if stamina low  // 这里是对体力的一个保护，体力过低就把Dash的dPower减小 保护体力
                    WM->getAgentStamina().getStamina() <
                    SS->getRecoverDecThr()*SS->getStaminaMax()+200 )
            {
                soc.dPower = 30.0 * WM->getAgentStamina().getRecovery(); // dash slow
                ACT->putCommandInQueue( soc );
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
            else                                           // if stamina high
            {
                ACT->putCommandInQueue( soc );               // dash as intended
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
        }
        else if(WM->getSecondClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL,(double*) &iTmp ) == WM->getAgentObjectType()  && 
	  WM->isDeadBallThem()&&( WM->getBallPos().getDistanceTo(WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL, &iTmp ))>SS->getMaximalKickDist()))
	{
	              //Log.log( 100, "I am second  to ball; can get there in %d cycles", iTmp );
            soc = intercept( false );                      // intercept the ball
            if ( soc.commandType == CMD_DASH &&            // if stamina low  // 这里是对体力的一个保护，体力过低就把Dash的dPower减小 保护体力
                    WM->getAgentStamina().getStamina() <
                    SS->getRecoverDecThr()*SS->getStaminaMax()+200 )
            {
                soc.dPower = 30.0 * WM->getAgentStamina().getRecovery(); // dash slow
                ACT->putCommandInQueue( soc );
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
            else                                           // if stamina high
            {
                ACT->putCommandInQueue( soc );               // dash as intended
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
	}
        else if ( posAgent.getDistanceTo(WM->getStrategicPosition()) >
                  1.5 + fabs(posAgent.getX()-posBall.getX())/10.0) // 到了这里就是其他距离球相对远一点的人了，如果离自己的阵形点太远，就跑回 自己的阵形点去。
            // if not near strategic pos
        {
            if ( WM->getAgentStamina().getStamina() >    // if stamina high
                    SS->getRecoverDecThr()*SS->getStaminaMax()+800 )
            {
                soc = moveToPos(WM->getStrategicPosition(),
                                PS->getPlayerWhenToTurnAngle());
                ACT->putCommandInQueue( soc );            // move to strategic pos
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
            else                                        // else watch ball
            {
                ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
        }
        else if ( fabs( WM->getRelativeAngle( OBJECT_BALL ) ) > 1.0 ) // watch ball //其他剩下的球员呢，就看球。！
        {///这里就是无球队员的跑位决策
            ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
        }
        else                                         // nothing to do
            ACT->putCommandInQueue( SoccerCommand(CMD_TURNNECK,0.0) );
    }
    return soc;
}

/*!This method is a simple goalie based on the goalie of the simple Team of
   FC Portugal. It defines a rectangle in its penalty area and moves to the
   position on this rectangle where the ball intersects if you make a line
   between the ball position and the center of the goal. If the ball can
   be intercepted in the own penalty area the ball is intercepted and catched.
*/
SoccerCommand Player::deMeer5_goalie(  )
{
    int i;
    SoccerCommand soc;
    VecPosition   posAgent = WM->getAgentGlobalPosition();
    AngDeg        angBody  = WM->getAgentGlobalBodyAngle();

    // define the top and bottom position of a rectangle in which keeper moves
    static const VecPosition posLeftTop( -PITCH_LENGTH/2.0 +
                                         0.7*PENALTY_AREA_LENGTH, -PENALTY_AREA_WIDTH/4.0 );
    static const VecPosition posRightTop( -PITCH_LENGTH/2.0 +
                                          0.7*PENALTY_AREA_LENGTH, +PENALTY_AREA_WIDTH/4.0 );

    // define the borders of this rectangle using the two points.
    static Line  lineFront = Line::makeLineFromTwoPoints(posLeftTop,posRightTop);
    static Line  lineLeft  = Line::makeLineFromTwoPoints(
                                 VecPosition( -50.0, posLeftTop.getY()), posLeftTop );
    static Line  lineRight = Line::makeLineFromTwoPoints(
                                 VecPosition( -50.0, posRightTop.getY()),posRightTop );
    
        if(WM->isFreeKickThem())
    {
                  VecPosition posMyGoal = ( WM->getSide() == SIDE_LEFT )
                                    ? SoccerTypes::getGlobalPositionFlag(OBJECT_GOAL_L, SIDE_LEFT )
                                    : SoccerTypes::getGlobalPositionFlag(OBJECT_GOAL_R, SIDE_RIGHT);
         Line lineBall = Line::makeLineFromTwoPoints( WM->getBallPos(),posMyGoal);
                 VecPosition posIntersect = lineFront.getIntersection( lineBall );

            // outside rectangle, use line at side to get intersection
            if (posIntersect.isRightOf( posRightTop ) )
                posIntersect = lineRight.getIntersection( lineBall );
            else if (posIntersect.isLeftOf( posLeftTop )  )
                posIntersect = lineLeft.getIntersection( lineBall );

            if ( posIntersect.getX() < -49.0 )
                posIntersect.setX( -49.0 );

     soc = moveToPos((52,3), PS->getPlayerWhenToTurnAngle());
      ACT->putCommandInQueue( soc );
      ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }

    if ( WM->isBeforeKickOff( ) )
    {
        if ( formations->getFormation() != FT_INITIAL || // not in kickoff formation
                posAgent.getDistanceTo( WM->getStrategicPosition() ) > 2.0 )
        {
            formations->setFormation( FT_INITIAL );       // go to kick_off formation
            ACT->putCommandInQueue( soc=teleportToPos(WM->getStrategicPosition()) );
        }
        else                                            // else turn to center
        {
            ACT->putCommandInQueue( soc = turnBodyToPoint( VecPosition( 0, 0 ), 0 ));
            ACT->putCommandInQueue( alignNeckWithBody( ) );
        }
        return soc;
    }


    if ( WM->getConfidence( OBJECT_BALL ) < PS->getBallConfThr() )
    {                                                // confidence ball too  low
        ACT->putCommandInQueue( searchBall() );        // search ball
        ACT->putCommandInQueue( alignNeckWithBody( ) );
    }
    else if ( WM->getPlayMode() == PM_PLAY_ON || WM->isFreeKickThem() ||
              WM->isCornerKickThem() )
    {
        if ( WM->isBallCatchable() )
        {
            ACT->putCommandInQueue( soc = catchBall() );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
        }
        else if ( WM->isBallKickable() )
        {
            soc = kickTo( WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_TEAMMATE_1), 2.0 );
            ACT->putCommandInQueue( soc );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
        }
        else if ( WM->isInOwnPenaltyArea( getInterceptionPointBall( &i, true ) ) &&
                  WM->getFastestInSetTo( OBJECT_SET_PLAYERS, OBJECT_BALL, &i ) ==
                  WM->getAgentObjectType() )
        {
            ACT->putCommandInQueue( soc = intercept( true ) );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
        }
        else
        {
            // make line between own goal and the ball
            VecPosition posMyGoal = ( WM->getSide() == SIDE_LEFT )
                                    ? SoccerTypes::getGlobalPositionFlag(OBJECT_GOAL_L, SIDE_LEFT )
                                    : SoccerTypes::getGlobalPositionFlag(OBJECT_GOAL_R, SIDE_RIGHT);
            Line lineBall = Line::makeLineFromTwoPoints( WM->getBallPos(),posMyGoal);

            // determine where your front line intersects with the line from ball
            VecPosition posIntersect = lineFront.getIntersection( lineBall );

            // outside rectangle, use line at side to get intersection
            if (posIntersect.isRightOf( posRightTop ) )
                posIntersect = lineRight.getIntersection( lineBall );
            else if (posIntersect.isLeftOf( posLeftTop )  )
                posIntersect = lineLeft.getIntersection( lineBall );

            if ( posIntersect.getX() < -49.0 )
                posIntersect.setX( -49.0 );

            // and move to this position
            if ( posIntersect.getDistanceTo( WM->getAgentGlobalPosition() ) > 0.5 )
            {
                soc = moveToPos( posIntersect, PS->getPlayerWhenToTurnAngle() );
                ACT->putCommandInQueue( soc );
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
            else
            {
                ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
                ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
            }
        }
    }
    else if ( WM->isFreeKickUs() == true || WM->isGoalKickUs() == true )
    {
        if ( WM->isBallKickable() )
        {
	    
            if ( WM->getTimeSinceLastCatch() == 25 && WM->isFreeKickUs() )
            {
                // move to position with lesser opponents.
                if ( WM->getNrInSetInCircle( OBJECT_SET_OPPONENTS,
                                             Circle(posRightTop, 15.0 )) <
                        WM->getNrInSetInCircle( OBJECT_SET_OPPONENTS,
                                                Circle(posLeftTop,  15.0 )) )
                    soc.makeCommand( CMD_MOVE,posRightTop.getX(),posRightTop.getY(),0.0);
                else
                    soc.makeCommand( CMD_MOVE,posLeftTop.getX(), posLeftTop.getY(), 0.0);
                ACT->putCommandInQueue( soc );
            }
            else if ( WM->getTimeSinceLastCatch() > 28 )
            {
                soc = kickTo( WM->getSecondClosestInSetTo(OBJECT_SET_TEAMMATES,OBJECT_TEAMMATE_1), 2.0 );
                ACT->putCommandInQueue( soc );
            }
        }
        else if ( WM->isGoalKickUs()  )
        {
            ACT->putCommandInQueue( soc = intercept( true ) );
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
        }
        else
            ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    else
    {
        ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
        ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    }
    return soc;
}