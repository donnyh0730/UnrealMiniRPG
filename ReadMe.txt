안녕하세요. 한동웅 입니다.

언리얼엔진 포트폴리오의 소스코드를 공개합니다.
우선 중점적으로 보셔야할 것은 

메인 케릭터 : Main.h ,Main.cpp
몬스터 : Enemy.h, Enemy.cpp
보스몬스터 : Enemy_Boss.h, Enemy_Boss.cpp
무기 : Weapon.h, Weapon.cpp
위젯 : Widget이라는 접두어가 붙은 헤더파일과 cpp파일들 

정도만 봐주시면 될것 같습니다. 

영상에 보신 전체적인 구현내용 정리.

1. 3인칭 SoulLike게임 케릭터 애니메이션
	-AnimationBlueprint , AnimationBlendSpace, AminMontage

2. 몬스터 Behaviour 
	AIController , BehaviourTree , C++전투 로직

3. 메인케릭터 공격, 회피기, 스프린트, 스킬 구현

4. 무기아이템 구현
	- 장착 , 탈착.
	- 무기스탯 공격력과 공격스피드에 적용.

5. 보스몬스터 구현 
	- 일반 몬스터 클래스를 상속받아 구현,
	- 전투방식, 공격패턴 다양화 (ex : 보스는 넉백이 없고, 공격속도가 느리지만 공격력이 강하다.)
	- 공격모션 3가지, 필살기 1가지

6.지형 지물 , Switch 액터,  아이템 , 포션 구현

7. 위젯  구현 (C++)

English

Hello. This is Dongwoong Han.

The source code of the Unreal Engine portfolio is released.
First of all, the focus is on

Main character: Main.h, Main.cpp
Monster: Enemy.h, Enemy.cpp
Boss Monster: Enemy_Boss.h, Enemy_Boss.cpp
Weapon: Weapon.h, Weapon.cpp
Widget: Header file and cpp files prefixed with Widget

I think you only need to look at the degree.

Summary of the overall implementation you saw in the video.

1. 3rd person SoulLike game character animation
	-AnimationBlueprint, AnimationBlendSpace, AminMontage

2. Monster Behaviour
	-AIController, BehaviorTree, C++ combat logic

3. Main character attack, evasion, sprint, skill implementation

4. Weapon item implementation
	-Attachment and detachment.
	-Applied to weapon stat attack power and attack speed.

5. Boss monster implementation
	-Implemented by inheriting the general monster class,
	-Diversification of combat method and attack pattern (ex: boss has no knockback, attack speed is slow, but attack power is strong)
	-3 attack motions, 1 special move
6. Terrain feature, switch actor, item, potion implementation

7. Widget implementation (C++)


